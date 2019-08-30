/**
** main.c
** Normal Algorithm in OpenMP
**
** Created by Aruna Bellgutte Ramesh on 22/04/19.
** Copyright © 2019 Aruna Bellgutte Ramesh. All rights reserved.
**/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include <time.h>

#define SIZE 65536
#define SIZEBY10 (int)SIZE/10
long num[SIZE];

void getArrayOfNumbers(){
    for(long i=0;i<SIZE;i++){
        num[i] = i+1;
    }
    return;
}

/* computes the sum of array of values sequentially */
void computeSequentialSum(){
    long sum = 0;
    double time;
    time = omp_get_wtime();
    for(long i=0;i<SIZE;i++){
        sum = sum + num[i];
    }
    time = omp_get_wtime() - time;
    printf("\n\tSum is %ld.  Time taken to compute sum of array values sequentially is %f seconds.\n",sum,time);
    return;
}

/* computes the sum of array of values parallelly using loop construct */
void computeParallelSumUsingLoopConstruct(){
    long sum = 0;
    double time;
    time = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum)
        for(long i=0;i<SIZE;i++){
            sum = sum + num[i];
        }
    time = omp_get_wtime() - time;
    printf("\n\tSum is %ld.  Time taken to compute sum of array values parallelly using loop construct/directive is %f seconds.\n",sum,time);
    return;
}

long computeTask(start,end){
    long i,diff;
    long sum = 0, sum1, sum2;
    if(end-start<SIZEBY10){
        for(i=start;i<end;i++){
            sum = sum + num[i];
        }
    }else{
        diff = end - start;
        #pragma omp task shared(sum1)
            sum1 = computeTask(start,(end-diff/2)); 
        #pragma omp task shared(sum2)
            sum2 = computeTask((end-diff/2),end);
        #pragma omp taskwait
            sum = sum1 + sum2;
    }
    return sum;
}

/* computes the sum of array of values parallelly using task construct */
void computeParallelSumUsingTaskConstruct(){
    long sum;
    double time;
    time = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
            sum = computeTask(0,SIZE);
        }
    }
    time = omp_get_wtime() - time;
    printf("\n\tSum is %ld.  Time taken to compute sum of array values parallelly using task construct/directive is %f seconds.\n",sum,time);
    return;
}

int main ()
{ 
    getArrayOfNumbers();
    omp_set_num_threads(16);

    printf("**************************************************************************************************************************");
    printf("\n");
    printf("\nResults for Normal Algorithm:\n");
    printf("\nSize of Array is %ld\n",(long)SIZE);
    printf("\nResults for Sequential Summation:\n");
    computeSequentialSum();
    printf("\nResults for Parallel Summation:\n");
    computeParallelSumUsingLoopConstruct();
    computeParallelSumUsingTaskConstruct();
    printf("\n");
    printf("**************************************************************************************************************************");
    printf("\n");
    printf("\n%d\n",SIZEBY10);
    return 0;
}