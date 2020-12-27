### Result of execution:

<p align="center">
  <img src="https://i.ibb.co/TWLkMMh/omp.png" title="hover text">
</p>

The program has to be compiled with *-fopenmp* option for OMP to work. </br>
Our task it to find **dot product of two vectors** (5 variant -> 1 task). </br> It can be simply done by creating a single *for-loop* and using one formula: ` dotProduct = dotProduct + firstVector[i] * secondVector[i].`
We create two dynamic arrays for that and run it both with OpenMP and without. Running it without OpenMP is the same as setting here *omp_set_num_threads(desiredThreads)* desiredThreads to 1. The output shows execution involving all threads one-by-one (you can actually hear this process with your own ears). </br>
I am going to briefly explain the meaning of OMP instructions and pragma directives that were used in the program:
`omp_get_max_threads` - the maximum number of threads that your processor supports </br>
`omp_set_num_threads` - set desired number of threads </br>
`omp_get_wtime` - it is specifically used to measure the time of the execution </br>
`#pragma omp parallel` - instructs the compiler to parallelize the chosen block of code </br>
`#pragma omp for` - distributes loop iterations within some number of threads </br>
[Click here](https://www.ibm.com/support/knowledgecenter/SSGH3R_16.1.0/com.ibm.xlcpp161.aix.doc/compiler_ref/tuoptppp.html) if you want to learn more about pragma directives for parallel processing and [here](https://www.openmp.org/spec-html/5.0/openmpch3.html#x144-6340003) to get acquainted with OMP runtime library routines.
