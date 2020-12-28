#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>

std::mutex mutexLock;
uint32_t numOfTasks = 1024*1024;
short sleep;

template <class T>
void CheckIfArrayWasCorrectlyPopulated(std::vector<T> tasks) 
{
    unsigned int count = 0;
    for (auto task : tasks) 
    {
        if (task != 1) 
        {
            std::cout << "You failed!" << std::endl;
        }
    }
}

void MutexIncrementer() 
{
    uint32_t counter = 0;
    std::vector<uint32_t> tasks(numOfTasks, 0);
    while (counter < numOfTasks) 
    {
        std::lock_guard<std::mutex> lockGuard(mutexLock);
        uint32_t helper = counter++;
        if (helper < numOfTasks)
        {
            tasks.at(helper) += 1;
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
        }       
    }
    CheckIfArrayWasCorrectlyPopulated(tasks);
}

void AtomicIncrementer() 
{
    std::vector<uint32_t> tasks(numOfTasks, 0);
    std::atomic<uint32_t> atomicCounter;
    while (atomicCounter < numOfTasks) 
    {
        tasks.at(atomicCounter) += 1;
        atomicCounter.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep));
    }
    CheckIfArrayWasCorrectlyPopulated(tasks);
}

void ChooseThreadSafety(char lockOrLockFree, uint32_t numOfThreads)
{
    std::vector<std::thread> threads;

    if (lockOrLockFree == 'm') 
    {
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
        std::cout << "Execution Time: " << executionTime.count() << " milliseconds. " << "Threads used: " << numOfThreads << "\n";
    }

    if (lockOrLockFree == 'a')
    {
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
        std::cout << "Execution Time: " << executionTime.count() << " milliseconds. " << "Threads used: " << numOfThreads << "\n";
        std::cout << std::endl;
    }
}

int main() 
{
    std::vector<uint32_t> numOfThreads = {4, 8, 16, 32};

    for (auto threads : numOfThreads)
    {
        std::cout << "Without sleep: " << std::endl;
        sleep = 0;
        ChooseThreadSafety('m', threads);   
        ChooseThreadSafety('a', threads);   

        std::cout << "Now with 10ms sleep: " << std::endl;
        sleep = 10;
        ChooseThreadSafety('m', threads);   
        ChooseThreadSafety('a', threads);

        std::cout << std::endl;
    }
    return 0;
}
