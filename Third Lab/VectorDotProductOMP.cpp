#include <iostream>
#include <omp.h>

int NotParallelDotProduct(double* firstVector, double* secondVector, long long sizeOfVector)
{
    double product = 0;   
    for (int i = 0; i < sizeOfVector; i++) 
    {
       product = product + firstVector[i] * secondVector[i]; 
    }
    return product;
}

int ParallelDotProduct(double* firstVector, double* secondVector, long long sizeOfVector)
{
    double pickupProduct;
    #pragma omp parallel
    {
        double product = 0;
        #pragma omp for
        for (int i = 0; i < sizeOfVector; i++) 
        {
            product = product + firstVector[i] * secondVector[i]; 
        }
        #pragma omp critical
        pickupProduct += product;
    }
    return pickupProduct;
}

int main() 
{ 
    long long sizeOfVector = 1000000;

    double* firstVector = new double[sizeOfVector];
    double* secondVector = new double[sizeOfVector];
    double dotParallelProduct, dotNotParallelProduct;

    for (int i = 0; i < sizeOfVector; i++)
    {
        firstVector[i] = rand()%1000;
        secondVector[i] = rand()%1000;
    }
    
    double start, end;
    int numOfThreads = omp_get_max_threads();

    for (int i = 1; i <= numOfThreads; i++)
    {
        omp_set_num_threads(i);
        start = omp_get_wtime();
        for (int j = 0; j < 1000; j++)
        {
            dotParallelProduct = ParallelDotProduct(firstVector, secondVector, sizeOfVector);
        }
        end = omp_get_wtime();
        std::cout << "Thread#: " << i << " Execution Time: " << end - start << "\n";
    }

    start = omp_get_wtime();
    for (int j = 0; j < 1000; j++)
    {
        dotNotParallelProduct = NotParallelDotProduct(firstVector, secondVector, sizeOfVector);
    }
    end = omp_get_wtime();
    std::cout << "Ordinary Execution Time: " << end - start << "\n";

    if (dotParallelProduct == dotNotParallelProduct)
    {
        std::cout << "Products are equal!" << "\n";
    }
    else
    {
        std::cout << "That was a simple task and you failed. You gotta be ashamed of yourself" << "\n";
    }
    
    delete[] firstVector;
    delete[] secondVector;
    
    return 0; 
} 