#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>

std::mutex mutexLock;
uint32_t numOfTasks = 1024 * 1024;
short sleep;

template <class T>
void CheckIfArrayWasPopulatedCorrectly(std::vector<T> tasks) 
{
    unsigned int count = 0;
    for (auto task : tasks) 
    {
        if (task != 1) 
        {
            std::cout << "You failed!" << "\n";
        }
    }
}

void ChooseThreadSafety(char lockOrLockFree, uint16_t numOfThreads)
{
    std::vector<std::thread> threads;
    std::vector<uint8_t> tasks(numOfTasks, 0);
    uint32_t mutexCounter = 0;
    std::atomic<uint32_t> atomicCounter(0);

    if (lockOrLockFree == 'm') 
    {
        auto MutexIncrementer = [&]()
        {
            while (mutexCounter < numOfTasks) 
            {
                std::lock_guard<std::mutex> lockGuard(mutexLock);
                uint32_t helperCounter = mutexCounter++;
                if (helperCounter < numOfTasks)
                {
                    tasks.at(helperCounter) += 1;
                    std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
                }       
            }
        };

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numOfThreads; i++) 
        {
            threads.push_back(std::thread(MutexIncrementer));
        }

        for (std::thread &t : threads) 
        {
            t.join();
        }

        auto finish = std::chrono::high_resolution_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
        std::cout << "Mutex Execution Time: " << executionTime.count() << " milliseconds. " << "Threads used: " << numOfThreads << "\n";

        CheckIfArrayWasPopulatedCorrectly(tasks);
    }

    if (lockOrLockFree == 'a')
    {
        auto AtomicIncrementer = [&]()
        {
            while (atomicCounter < numOfTasks) 
            {
                uint32_t helperCounter = atomicCounter.fetch_add(1);
                if (helperCounter < numOfTasks)
                {
                    tasks.at(helperCounter) += 1;
                    std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
                }
            }
        };

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numOfThreads; i++) 
        {
            threads.push_back(std::thread(AtomicIncrementer));
        }

        for (std::thread &t : threads) 
        {
            t.join();
        }

        auto finish = std::chrono::high_resolution_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
        std::cout << "Atomic Execution Time: " << executionTime.count() << " milliseconds. " << "Threads used: " << numOfThreads << "\n";
        std::cout << "\n";

        CheckIfArrayWasPopulatedCorrectly(tasks);
    }
}

int main() 
{
    std::vector<uint16_t> numOfThreads = {4, 8, 16, 32};

    for (auto threads : numOfThreads)
    {
        std::cout << "Without sleep: " << "\n";
        sleep = 0;
        ChooseThreadSafety('m', threads);   
        ChooseThreadSafety('a', threads);   

        std::cout << "Now with 10ms sleep: " << "\n";
        sleep = 10;
        ChooseThreadSafety('m', threads);   
        ChooseThreadSafety('a', threads); 

        std::cout << "\n";
    }
    return 0;
}
