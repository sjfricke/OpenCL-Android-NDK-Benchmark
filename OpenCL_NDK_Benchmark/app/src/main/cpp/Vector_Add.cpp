#include "OpenCL_Benchmark.h"

double OpenCL_Benchmark::Vector_Add(void) {

    clock_t start_t, end_t;
    double  time_t;
    start_t = clock(); // Start Time!

    // Length of vectors
    unsigned int n = 1000000;

    // Host input vectors
    double *h_a;
    double *h_b;
    // Host output vector
    double *h_c;

    // Device input buffers
    cl_mem d_a;
    cl_mem d_b;
    // Device output buffer
    cl_mem d_c;

    cl_platform_id cpPlatform;        // OpenCL platform
    cl_device_id device_id;           // device ID
    cl_context context;               // context
    cl_command_queue queue;           // command queue
    cl_program program;               // program
    cl_kernel kernel;                 // kernel

    // Size, in bytes, of each vector
    size_t bytes = n * sizeof(double);

    // Allocate memory for each vector on host
    h_a = (double *) malloc(bytes);
    h_b = (double *) malloc(bytes);
    h_c = (double *) malloc(bytes);

    // Initialize vectors on host
    int i;
    for (i = 0; i < n; i++) {
        h_a[i] = 0 + i;
        h_b[i] = 0 - i;
    }

    size_t globalSize, localSize;
    cl_int err;

    // Number of work items in each local work group
    localSize = 64;

    // Number of total work items - localSize must be devisor
    globalSize = 100032;

    // Bind to platform
    err = clGetPlatformIDs(1, &cpPlatform, NULL);

    // Get ID for the device
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    // Create a context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

    // Create a command queue
    queue = clCreateCommandQueue(context, device_id, 0, &err);

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1,
                                        (const char **) &kernel_source, NULL, &err);

    // Build the program executable
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, "vecAdd", &err);

    // Create the input and output arrays in device memory for our calculation
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);

    // Write our data set into the input array in device memory
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,
                               bytes, h_a, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0,
                                bytes, h_b, 0, NULL, NULL);

    // Set the arguments to our compute kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);

    // Execute the kernel over the entire range of the data set
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize,
                                 0, NULL, NULL);

    // Wait for the command queue to get serviced before reading back results
    clFinish(queue);

    // Read the results from the device
    clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
                        bytes, h_c, 0, NULL, NULL);


    // Ends timing since rest is validation and cleanup
    end_t = clock();
    time_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    //Sum up vector c and print result divided by n, this should equal 0 within error
    double sum = 0;
    for (i = 0; i < n; i++) {
        sum += h_c[i];
    }
    double result = (sum / (double) n);
    LOGI("Vector Add - Should be 0.000 - Result: %f", result);

    // release OpenCL resources
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    //release host memory
    free(h_a);
    free(h_b);
    free(h_c);

    return (result == 0.0) ? time_t : -1.0;
}