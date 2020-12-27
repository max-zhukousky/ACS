#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <future>
#include <queue>

class MainQueue
{ 
    public:
    virtual void Push(uint8_t value) = 0;
    virtual bool Pop(uint8_t& value) = 0;
};

class ClassicQueue : public MainQueue
{
    std::mutex mutexLock;
    std::queue<uint8_t> classicQueue;

    public:
    void Push(uint8_t value) override;
    bool Pop(uint8_t& value) override;
};

void ClassicQueue::Push(uint8_t value) 
{
    std::lock_guard<std::mutex> lockGuard(mutexLock);
    classicQueue.push(value);
}

bool ClassicQueue::Pop(uint8_t& value)
{
    std::lock_guard<std::mutex> lockGuard(mutexLock);
    if (classicQueue.empty() == true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return false;
    }
    value = classicQueue.front();
    classicQueue.pop();
    return true;
}

class MutexQueue : public MainQueue
{
    std::mutex mutexLock;
    int queueSize;
    std::condition_variable cvForPush, cvForPop;
    std::atomic<int> pushedElements, pushedIndex, poppedIndex;
    uint8_t mutexQueue[];

public:
    MutexQueue(int queueSize);
    void Push(uint8_t value) override;
    bool Pop(uint8_t& value) override;
};

MutexQueue::MutexQueue(int queueSize) : queueSize(queueSize)
{
    mutexQueue[queueSize] = { 0 };
    pushedElements = 0, pushedIndex = 0, poppedIndex = 0;
}

void MutexQueue::Push(uint8_t value)
{
    std::unique_lock<std::mutex> uniqueLock(mutexLock);
    while (pushedElements == queueSize) 
    {
        cvForPush.wait(uniqueLock);
    }
    auto front = pushedIndex % queueSize;
    mutexQueue[front] = value;
    pushedIndex++;
    pushedElements += 1;
    cvForPop.notify_one();
}

bool MutexQueue::Pop(uint8_t& value)
{
    std::unique_lock<std::mutex> uniqueLock(mutexLock);
    while (pushedElements == 0)
    {
        cvForPop.wait(uniqueLock);
    }
    auto front = poppedIndex % queueSize;
    value = mutexQueue[front];
    mutexQueue[front] = 0;
    poppedIndex++;
    pushedElements -= 1;
    cvForPush.notify_one();
    return true;
}

class AtomicQueue : public MainQueue
{
    int queueSize;
    std::atomic<int> front;
    std::atomic<int> pushedElement, poppedElement;
    std::atomic<uint8_t>* atomicQueue;
public:
    AtomicQueue(int queueSize);
    void Push(uint8_t value) override;
    bool Pop(uint8_t& value) override;
};

AtomicQueue::AtomicQueue(int queueSize) : queueSize(queueSize)
{
    atomicQueue = new std::atomic<uint8_t>[queueSize];
    for (int i = 0; i < queueSize; i++)
    {
        atomicQueue[i] = 0;
    }
    front = 0;
    pushedElement, poppedElement = 0;
}

void AtomicQueue::Push(uint8_t value)
{
    while (true) 
    {
        if (value > 256) throw std::invalid_argument("Value cannot be more than 1 byte");
        int nextElementToPush = pushedElement.load(std::memory_order_acquire);
        uint8_t chosenSlot = atomicQueue[nextElementToPush % queueSize];
        if (chosenSlot == 0) {
            if (pushedElement.compare_exchange_strong(nextElementToPush, nextElementToPush + 1))
            {
                atomicQueue[nextElementToPush % queueSize].compare_exchange_strong(chosenSlot, value);
                return;
            }
        }
        else continue;
    }
}

bool AtomicQueue::Pop(uint8_t& value) 
{
    int nextElementToPop = poppedElement.load(std::memory_order_acquire);
    if (nextElementToPop == pushedElement.load(std::memory_order_acquire)) return false;
    uint8_t chosenSlot = atomicQueue[nextElementToPop % queueSize];
    if (chosenSlot != 0)
    {
        if (poppedElement.compare_exchange_strong(nextElementToPop, nextElementToPop + 1)) 
        {
            atomicQueue[nextElementToPop % queueSize].compare_exchange_strong(chosenSlot, 0);
            value = chosenSlot;
            return true;
        }
    }
    else
    {
        return false;
    }
    return false; // Queue is finally empty
}

void ProducingAndConsuming(MainQueue& queue, short numOfProducers, short numOfConsumers, uint32_t numOfTasks)
{
    std::vector<std::thread> threads;
    std::mutex mutexLock;
    uint32_t sumCounter = 0;

    auto producer = [&]() 
    {
        for (int i = 0; i < numOfTasks; i++) 
        {
            queue.Push(1);
        }
    };

    for (int i = 0; i < numOfProducers; i++)
    {
        threads.push_back(std::thread(producer));
    }

    auto consumer = [&]() 
    {
        for(int i = 0; i < numOfTasks * numOfProducers / numOfConsumers; i++) 
        {
            std::lock_guard<std::mutex> lockGuard(mutexLock);
            uint8_t front = 0;
            while (!queue.Pop(front));
            sumCounter += front;
        }
    };

    for (int i = 0; i < numOfConsumers; i++)
    {
        threads.push_back(std::thread(consumer));
    }

    for (std::thread &t : threads)
    {
        t.join();
    }

    uint32_t executionCondition = numOfTasks * numOfProducers;
    std::cout << "Number of Producers: " << numOfProducers << " Number of Consumers: " << numOfConsumers;
    if (sumCounter == executionCondition) 
    {
        std::cout << " Passed" << "\n";
    }
}

int main()
{
    std::vector<int> queueSizes = {1, 4, 16};
    std::vector<int> numOfProducers = {1, 2, 4};
    std::vector<int> numOfConsumers = {1, 2, 4};
    int numOfTasks = 4 * 1024 * 1024;

    ClassicQueue classicQueue;
    for (auto producer : numOfProducers)
    {
        for (auto consumer : numOfConsumers)
        {
            auto start = std::chrono::high_resolution_clock::now();
            ProducingAndConsuming(classicQueue, producer, consumer, numOfTasks);
            auto finish = std::chrono::high_resolution_clock::now();
            auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
            std::cout << "Execution Time: " << executionTime.count() << " ms" << "\n" << "\n";
        }
    }
    
    for (auto queueSize : queueSizes)
    {
        MutexQueue mutexQueue(queueSize);
        std::cout << "Queue size: " << queueSize << std::endl;
        for (auto producer : numOfProducers)
        {
            for (auto consumer : numOfConsumers)
            {
                auto start = std::chrono::high_resolution_clock::now();
                ProducingAndConsuming(mutexQueue, producer, consumer, numOfTasks);
                auto finish = std::chrono::high_resolution_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
                std::cout << "Execution Time: " << executionTime.count() << " ms" << "\n" << "\n";
            }
        }
    }

    for (auto queueSize : queueSizes) 
    {
        AtomicQueue atomicQueue(queueSize);
        std::cout << "Queue size: " << queueSize << std::endl;
        for (auto producer : numOfProducers)
        {
            for (auto consumer : numOfConsumers)
            {
                auto start = std::chrono::high_resolution_clock::now();
                ProducingAndConsuming(atomicQueue, producer, consumer, numOfTasks);
                auto finish = std::chrono::high_resolution_clock::now();
                auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
                std::cout << "Execution Time: " << executionTime.count() << " ms" << "\n" << "\n";
            }
        }
    }

    return 0;
}
