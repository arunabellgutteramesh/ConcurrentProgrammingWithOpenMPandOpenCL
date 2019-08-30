# ConcurrentProgrammingWithOpenMPandOpenCL
An efficient implementation of the Parallel Sum Reduction operator that computes the sum of a large array of values in both OpenMP and OpenCL.

# Assignment Problem Statement
### Parallel Sum Reduction in OpenMP and OpenCL
In this assignment one has to develop an efficient implemention of the Parallel Sum Reduction operator that computes the sum of a large array of values in both OpenMP and OpenCL. A prime criterion in the assignment will be the efficiency of both the implementations and the evidence presented to substantiate claims that implementations are efficient.

# Solution
### OpenMP

#### Solution 1: 
Parallel Directives Used:
1. OpenMP parallel for construct
OpenMP parallel for construct was used to parallelize the summation. It was
observed that, for a typical large array with array size greater than or equal to 2^20,
the time taken to execute the program using a parallel for construct is less than serial
execution of the same. For example, for an array size of 2^29, a parallel summation
with parallel for construct takes about 0.672336 seconds only. However, when
compared between parallel for construct and parallel task construct, we can find some
interesting findings which will be discussed under the parallel task construct.

2. OpenMP parallel task construct
Along with OpenMP parallel for construct, OpenMP parallel task construct too was
used to parallelize the summation. Like parallel for construct, parallel task 
construct too reduces the time of summation when array size is greater 
than or equal to 2^20. For example, for an array size of 2^29, a
parallel summation with parallel task construct takes about 0.599212 seconds only.

**Typically parallel task constructs are more efficient than parallel for construct. But
for array sizes greater than or equal to 2^30, parallel task construct degrades in
performance compared to parallel for construct.**

#### Solution 2: Parallel Sum Reduction Algorithm in OpenMP

In this algorithm, the program is implemented such that rather than a simple linear
summation of array values, the summation is done as in the below algorithm:

Step 1: A “slider” value is calculated as (veryLongArraySize/2).
Step 2: An array value at (i)th location is added with an array value of (i+slider)th location
and stored back to (i)th location. Now half the array values are processed stored. So now
this half should be processed again.
Step 3: Reduce array size, veryLongArraySize, by 2 and continue with step 1. If array size is
1 go to step 4.
Step 4: Value at veryLongArray[0] gives the sum of the very large array values.

This algorithm was implemented in OpenMP framework. 
This program shows the results for normal linear summation, parallel sum
reduction algorithm without any OpenMP constructs and parallel sum reduction algorithm
with parallel for construct. 

It can be observed from the results that parallel sum reduction algorithm is not suited for
OpenMP framework. The results shows that the algorithm takes more time than serial
summation. 

### OpenCL

#### Solution: Parallel Sum Reduction Algorithm in OpenCL

The Parallel Sum Reduction Algorithm, explained above, is best suited for OpenCL
framework. The algorithm was implemented with WorkerItems equal to the size of very
large array. GroupSize was set to 256. Also, GroupSize was evenly dividing WorkerItems.
The results of this algorithm as run on GPU is tabulated in table 3.

It can be observed from the results that for small array sizes, less than 2^20, parallel
execution takes longer than serial execution. But for sizes above and equal to 2^20, parallel
execution takes shorter time than serial execution. The results exponentially get better as the
array size increases. For example, for array size 2^30, serial execution took 15.65 seconds
but parallel execution took only 0.53 seconds. These results are way better than OpenMP
framework. However, OpenCL framework comes with an overhead of setting up the
environment and setting up the kernel. 

# Conclusion / Justifications
Owing to the test cases and screenshots, the study can be summarized as below:
1. For OpenMP framework and very large array sizes, above 2^30 array size, Parallel
For construct is best.
2. For smaller array sizes, between 2^20 and 2^30 array size, OpenMP Parallel Task
construct is better.
3. Parallel Sum Reduction Algorithm is best suited for OpenCL framework.
Implementing the algorithm on OpenMP framework only degrades the results.
4. OpenCL framework gives better results than OpenMP, for computing summation of
very large arrays, typically above 2^30 size. But it comes with the cost of
environment setup time.
5. May be for array sizes way too greater than 2^30 and the right hardware, OpenCL
will be better suited than OpenMP for summation of very large array values.
However, this cannot be verified in our local systems and hence is an open ended
observation.

# Report

For a detailed report, please refer the report in the repo.
