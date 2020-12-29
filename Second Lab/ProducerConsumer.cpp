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
    virtual bool IsEmpty() = 0;
};

class ContainerQueue : public MainQueue
{
    std::mutex mutexLock;
    std::queue<uint8_t> containerQueue;

    public:
    void Push(uint8_t value) override;
    bool Pop(uint8_t& value) override;
    bool IsEmpty() override;
};

void ContainerQueue::Push(uint8_t value) 
{
    std::lock_guard<std::mutex> lockGuard(mutexLock);
    containerQueue.push(value);
}

bool ContainerQueue::Pop(uint8_t& value)
{
    std::unique_lock<std::mutex> uniqueLock(mutexLock);
    if (containerQueue.empty())
    {
        uniqueLock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        uniqueLock.lock();
        if (containerQueue.empty()) 
        {
            return false;
        }
    }
    value = containerQueue.front();
    containerQueue.pop();
    return true;
}

bool ContainerQueue::IsEmpty()
{
    return containerQueue.empty();
}

class MutexQueue : public MainQueue
{
    std::mutex mutexLock;
    uint8_t queueSize;
    std::condition_variable cvForPush, cvForPop;
    uint8_t front, back, currentQueueSize;
    uint8_t* mutexQueue;

public:
    MutexQueue(uint8_t queueSize);
    ~MutexQueue();
    void Push(uint8_t value) override;
    bool Pop(uint8_t& value) override;
    int Front();
    int CurrentQueueSize();
    bool IsEmpty();
    bool IsFull();
};

MutexQueue::MutexQueue(uint8_t _queueSize)
{
    mutexQueue = new uint8_t[_queueSize];
    queueSize = _queueSize;
    front = 0, back = -1, currentQueueSize = 0;
}

MutexQueue::~MutexQueue()
{
    delete[] mutexQueue;
}

void MutexQueue::Push(uint8_t value)
{
    std::unique_lock<std::mutex> uniqueLock(mutexLock);
    cvForPush.wait(uniqueLock, [this] { return queueSize > CurrentQueueSize(); });
    back = (back + 1) % queueSize;
    mutexQueue[back] = value;
    currentQueueSize++;
    cvForPop.notify_one();
}

bool MutexQueue::Pop(uint8_t& value)
{
    std::unique_lock<std::mutex> uniqueLock(mutexLock);
    if (cvForPop.wait_for(uniqueLock, std::chrono::milliseconds(1),
    [this] { return CurrentQueueSize(); })) 
    {
        front = (front + 1) % queueSize;
        value = mutexQueue[front];
        currentQueueSize--;
        cvForPush.notify_one();
        return true;
    }
    return false;
}

int MutexQueue::Front()
{
    return mutexQueue[front];
}

int MutexQueue::CurrentQueueSize()
{
    return currentQueueSize;
}

bool MutexQueue::IsEmpty()
{
    std::unique_lock<std::mutex> uniqueLock(mutexLock);
    return (CurrentQueueSize() == 0);
}

bool MutexQueue::IsFull()
{
    return (CurrentQueueSize() == queueSize);
}

/* class AtomicQueue : public MainQueue
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
} */

void ProducingAndConsuming(MainQueue& queue, short numOfProducers, short numOfConsumers, uint32_t numOfTasks)
{
    std::vector<std::thread> producerThreads;
    std::vector<std::thread> consumerThreads;
    std::mutex mutexLock;
    uint32_t finalSum = 0;
    short activeProducers = numOfProducers;

    auto Producer = [&]() 
    {
        for (int i = 0; i < numOfTasks; i++) 
        {
            queue.Push(1);
        }
        std::lock_guard<std::mutex> lockGuard(mutexLock);
        activeProducers--;
    };

    auto Consumer = [&]() 
    {
        uint16_t interimSum = 0;
        while (activeProducers > 0 || !queue.IsEmpty())
        {
            uint8_t value;
            if (queue.Pop(value))
            {
                interimSum += value;
            }
        }
        std::lock_guard<std::mutex> lockGuard(mutexLock);
        finalSum += interimSum;
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numOfProducers; i++)
    {
        producerThreads.push_back(std::thread(Producer));
    }

    for (int i = 0; i < numOfConsumers; i++)
    {
        consumerThreads.push_back(std::thread(Consumer));
    }

    for (std::thread &t : producerThreads)
    {
        if (t.joinable()) 
        {
            t.join();
        }
    }

    for (std::thread &t : consumerThreads)
    {
        if (t.joinable()) 
        {
            t.join();
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << "\n";
    std::cout << "Execution Time: " << executionTime.count() << " ms" << " | ";

    uint32_t executionCondition = numOfTasks * numOfProducers;
    std::cout << "Number of Producers: " << numOfProducers << " | Number of Consumers: " << numOfConsumers;
    if (finalSum == executionCondition) 
    {
        std::cout << " | Passed" << "\n";
    }
    else std::cout << " Failed" << "\n";
}

int main()
{
    std::vector<int> queueSizes = {1, 4, 16};
    std::vector<int> numOfProducers = {1, 2, 4};
    std::vector<int> numOfConsumers = {1, 2, 4};
    int numOfTasks = 1024 * 10;

    ContainerQueue containerQueue;
    for (auto producer : numOfProducers)
    {
        for (auto consumer : numOfConsumers)
        {
            ProducingAndConsuming(containerQueue, producer, consumer, numOfTasks);
        }
    }
    
    for (auto queueSize : queueSizes)
    {
        MutexQueue mutexQueue(queueSize);
        std::cout << "\n" << "Queue size: " << queueSize;
        for (auto producer : numOfProducers)
        {
            for (auto consumer : numOfConsumers)
            {
                ProducingAndConsuming(mutexQueue, producer, consumer, numOfTasks);
            }
        }
    }

    /* for (auto queueSize : queueSizes) 
    {
        AtomicQueue atomicQueue(queueSize);
        std::cout << "Queue size: " << queueSize << "\n";
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
    } */

    return 0;
}
