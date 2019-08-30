//
//  main.c
//  ParallelSumReduction
//
//  Created by Aruna Bellgutte Ramesh on 22/04/19.
//  Copyright © 2019 Aruna Bellgutte Ramesh. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define INPUT_ARRAY_SIZE 1024 //size of input vector
#define WORK_GROUP_SIZE 256 //size of each work group
#define NUM_WORK_ITEMS INPUT_ARRAY_SIZE  //total number of work items (same as size of input vector)
#define NUM_WORK_GROUPS INPUT_ARRAY_SIZE/WORK_GROUP_SIZE //total number of work groups
#define MAX_SOURCE_SIZE (0x100000)

/* returns an array of numbers */
void getArrayOfNumbers(long *input_array,long *reductionSum_array)
{
    //iterator
    long i;
    //initialize input_array
    for (i = 0; i < NUM_WORK_ITEMS; i++)
    {
        input_array[i] = i + 1;
    }
    //initialize reductionSum_array
    for (i = 0; i < NUM_WORK_GROUPS; i++)
    {
        reductionSum_array[i] = 0;
    }
    return;
}

int main(void){
    
    cl_context context;
    cl_context_properties properties[3];
    cl_kernel kernel;
    cl_command_queue command_queue;
    cl_program program;
    cl_int err;
    cl_uint num_of_platforms = 0;
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_uint num_of_devices = 0;
    cl_mem input, reductionSum;
    
    //long input_array[INPUT_ARRAY_SIZE], reductionSum_array[NUM_WORK_GROUPS], i;
    long i;
    long* input_array;
    long* reductionSum_array;
    
    // Variables for clock
    struct timeval chrono1, chrono2;
    int micro_init, second_init;
    int micro_gpu_init, second_gpu_init;
    int micro_gpu, second_gpu;
    int micro_seq, second_seq;
    
    // Time start for preparing and initializing arrays
    gettimeofday(&chrono1, NULL);
    
    // input array
    input_array  = malloc(NUM_WORK_ITEMS*sizeof(long));
    
    // reduction sum array
    reductionSum_array = malloc(NUM_WORK_GROUPS*sizeof(long));
    
    getArrayOfNumbers(input_array,reductionSum_array);
    
    // Time end for preparing and initializing arrays
    gettimeofday(&chrono2, NULL);
    
    // Elapsed time for preparing and initializing arrays
    micro_init = chrono2.tv_usec - chrono1.tv_usec;
    if (micro_init < 0) {
        micro_init += 1000000;
        second_init = chrono2.tv_sec - chrono1.tv_sec - 1;
    }
    else
        second_init = chrono2.tv_sec - chrono1.tv_sec;
    
    // Time start for preparing GPU/OpenCL
    gettimeofday(&chrono1, NULL);
    
    //Load the source code of the program
    FILE *fp;
    //char fileName[] = "./sumReductionGPU.cl";
    char fileName[] = "/Users/arunabellgutteramesh/Documents/DCU/2ndSem/ConcurrentPrg/Assignment02/OpenCL/ParallelSumReduction/ParallelSumReduction/sumReductionGPU.cl";
    char *source_str;
    size_t source_size;
    fp = fopen(fileName, "r");
    if(!fp){
        fprintf(stderr, "Failed to load the file\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str,1,MAX_SOURCE_SIZE,fp);
    fclose(fp);
    
    //get the list of platforms available
    if(clGetPlatformIDs(1, &platform_id, &num_of_platforms)!=CL_SUCCESS)
    {
        printf("Unable to get Platform_Id\n");
        return 1;
    }
    //try to get a supported GPU device
    if(clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_of_devices)!=CL_SUCCESS)
    {
        printf("Unable to get device id\n");
        return 1;
    }
    //context properties list
    properties[0] = CL_CONTEXT_PLATFORM;
    properties[1] = (cl_context_properties)platform_id;
    properties[2] = 0;
    
    //create context with GPU device
    context = clCreateContext(properties, 1, &device_id, NULL,NULL, &err);
    
    //create command queue using context and device
    command_queue = clCreateCommandQueue(context, device_id, 0, &err);
    
    //create a program from kernel source code
    program = clCreateProgramWithSource(context, 1,(const char **)&source_str, (const size_t *)&source_size, &err);
    
    //compile program
    if(clBuildProgram(program, 0, NULL, NULL, NULL,NULL)!=CL_SUCCESS)
    {
        printf("Error building program\n");
        return 1;
    }
    
    //specify which kernel from the program to execute
    kernel = clCreateKernel(program, "sumGPU", &err);
    
    // Create buffers for the input and output
    input = clCreateBuffer(context, CL_MEM_READ_ONLY, NUM_WORK_ITEMS*sizeof(long), NULL, &err);
    reductionSum = clCreateBuffer(context, CL_MEM_READ_ONLY, NUM_WORK_GROUPS*sizeof(long), NULL, &err);
    
    //load data into the input buffer
    clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, NUM_WORK_ITEMS*sizeof(long), input_array, 0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, reductionSum, CL_TRUE, 0, NUM_WORK_GROUPS*sizeof(long), reductionSum_array, 0, NULL, NULL);
    
    // Set the arguments for the kernel
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&input);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&reductionSum);
    clSetKernelArg(kernel, 2, WORK_GROUP_SIZE*sizeof(long),NULL);
    
    // Time end for preparing GPU/OpenCL
    gettimeofday(&chrono2, NULL);
    
    // Elapsed time for preparing GPU/OpenCL
    micro_gpu_init = chrono2.tv_usec - chrono1.tv_usec;
    if (micro_gpu_init < 0) {
        micro_gpu_init += 1000000;
        second_gpu_init = chrono2.tv_sec - chrono1.tv_sec - 1;
    }
    else
        second_gpu_init = chrono2.tv_sec - chrono1.tv_sec;
    
    // GPU version
    // Time start for one GPU/NDRangeKernel call
    gettimeofday(&chrono1, NULL);
    
    size_t global_item_size = INPUT_ARRAY_SIZE;
    size_t local_item_size = WORK_GROUP_SIZE;
    
    // Execute kernel code
    if (clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL) != CL_SUCCESS)
        fprintf(stderr,"Error in clEnqueueNDRangeKernel\n");
    
    clFinish(command_queue);
    
    // Read the buffer back to the reductionSum_array
    if (clEnqueueReadBuffer(command_queue, reductionSum, CL_TRUE, 0, NUM_WORK_GROUPS*sizeof(long), reductionSum_array, 0, NULL, NULL) != CL_SUCCESS)
        fprintf(stderr,"Error in clEnqueueReadBuffer\n");
    
    // Final summation
    long finalParallelSum = 0;
    for (i=0; i<NUM_WORK_GROUPS; i++)
        finalParallelSum += reductionSum_array[i];
    
    // Time end for one GPU/NDRangeKernel call
    gettimeofday(&chrono2, NULL);
    
    // Elapsed time for one GPU/NDRangeKernel call
    micro_gpu = chrono2.tv_usec - chrono1.tv_usec;
    if (micro_gpu < 0) {
        micro_gpu += 1000000;
        second_gpu = chrono2.tv_sec - chrono1.tv_sec - 1;
    }
    else
        second_gpu = chrono2.tv_sec - chrono1.tv_sec;
    
    // Sequential version
    // Time start for computing sum
    gettimeofday(&chrono1, NULL);
    
    // Compute sequential sum
    long finalSequentialSum = 0;
    for (i=0; i<NUM_WORK_ITEMS; i++)
        finalSequentialSum += input_array[i];
    
    // Time end for computing sum
    gettimeofday(&chrono2, NULL);
    
    // Compute ellapsed time
    micro_seq = chrono2.tv_usec - chrono1.tv_usec;
    if (micro_seq < 0) {
        micro_seq += 1000000;
        second_seq = chrono2.tv_sec - chrono1.tv_sec - 1;
    }
    else
        second_seq = chrono2.tv_sec - chrono1.tv_sec;
    
    printf("\n");
    printf("\n");
    printf("  Results:\n");
    printf("\n");
    printf("  Size of the array is %ld\n",INPUT_ARRAY_SIZE);
    printf("\n");
    printf("  %ld is the sum calculated parallelly\n",finalParallelSum);
    printf("\n");
    printf("  %ld is the sum calculated serially\n",finalSequentialSum);
    printf("\n");
    printf("  Initializing Arrays : Wall Clock = %d second %d micro\n",second_init,micro_init);
    printf("\n");
    printf("  Preparing GPU/OpenCL : Wall Clock = %d second %d micro\n",second_gpu_init,micro_gpu_init);
    printf("\n");
    printf("  Time for one NDRangeKernel call and WorkGroups final Sum  : Wall Clock = %d second %d micro\n",second_gpu,micro_gpu);
    printf("\n");
    printf("  Time for Sequential Sum computing  : Wall Clock = %d second %d micro\n",second_seq,micro_seq);
    printf("\n");
    
    
    
    //release openCL resources
    clReleaseMemObject(input);
    clReleaseMemObject(reductionSum);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    //free dynamic resources
    free(source_str);
    free(input_array);
    free(reductionSum_array);
    
    //printf("%ld\n%ld\n",NUM_WORK_ITEMS,NUM_WORK_GROUPS);
    
    return 0;
}


