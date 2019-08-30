/**
** main.c
** ParallelSumReduction Algorithm in OpenMP
**
** Created by Aruna Bellgutte Ramesh on 22/04/19.
** Copyright © 2019 Aruna Bellgutte Ramesh. All rights reserved.
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define SIZE 1024
long num[SIZE];
long result[SIZE][SIZE];

/* returns an array of numbers */
void getArrayOfNumbers()
{
    for (long i = 0; i < SIZE; i++)
    {
        num[i] = i + 1;
        result[0][i] = num[i];
    }
    return;
}

/* computes the sum of array of values sequentially */
void computeSequentialSum()
{
    long sum = 0;
    double time;
    time = omp_get_wtime();
    for (long i = 0; i < SIZE; i++)
    {
        sum = sum + num[i];
    }
    time = omp_get_wtime() - time;
    printf("\n\tSum is %ld.  Time taken to compute sum of array values sequentially using normal algorithm is %f seconds.\n",sum,time);
    return;
}

/* computes the sum of array of values sequentially using Parallel Sum Reduction Algorithm*/
void computeParallelSumReductionSequentially()
{
    int reductor=1, stride=SIZE;
    long arr_end = log(SIZE)/log(2);
    long sum = 0, i, j;
    double time;
    time = omp_get_wtime();
    for(j=1;j<=arr_end;j++)
    {
        reductor = reductor*2;
        stride = stride/2;
        for(i=0;i<SIZE/reductor;i++)
        {
            result[j][i] = result[j-1][i] + result[j-1][i+stride];
        }
    }
    sum = result[arr_end][0];
    time = omp_get_wtime() - time;
    printf("\n\tSum is %ld.  Time taken to compute sum of array values sequentially but using ParallelSumReduction Algorithm is %f seconds.\n",sum,time);
    return;
}


/* computes the sum of array of values parallelly using Parallel Sum Reduction Algorithm and loop construct */
void computeParallelSumReductionUsingLoopConstruct(){
    double time;
    int reductor=1, stride=SIZE;
    long arr_end = log(SIZE)/log(2);
    long sum = 0, i, j;
    time = omp_get_wtime();
    for(j=1;j<=arr_end;j++)
    {
        reductor = reductor*2;
        stride = stride/2;
        #pragma omp parallel for
        for(i=0;i<SIZE/reductor;i++)
        {
            result[j][i] = result[j-1][i] + result[j-1][i+stride];
        }
    }
    sum = result[arr_end][0];
    time = omp_get_wtime() - time;
    printf("\n\tSum is %ld.  Time taken to compute sum of array values parallelly using ParallelSumReduction Algorithm and loop construct/directive is %f seconds.\n",sum,time);
    return;
}


int main()
{
    getArrayOfNumbers();
    omp_set_num_threads(16);

    printf("**************************************************************************************************************************");
    printf("\n");
    printf("\nResults for Parallel Sum Reduction Algorithm:\n");
    printf("\nSize of Array is %ld\n",(long)SIZE);
    printf("\nResults for Sequential Summation:\n");
    computeSequentialSum();
    computeParallelSumReductionSequentially();
    printf("\nResults for Parallel Summation:\n");
    computeParallelSumReductionUsingLoopConstruct();
    printf("\n");
    printf("**************************************************************************************************************************");
    printf("\n");
    return 0;
}