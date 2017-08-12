#include "OpenCL_Benchmark.h"

OpenCL_Benchmark::OpenCL_Benchmark() {

};

OpenCL_Benchmark::~OpenCL_Benchmark() {
    // make sure we don't leak native windows
    if (m_native_window != NULL) {
        ANativeWindow_release(m_native_window);
        m_native_window = NULL;
    }

    if (m_frame_buffer != NULL) {
        free(m_frame_buffer);
        m_frame_buffer = NULL;
    }
}

void OpenCL_Benchmark::OnCreate(JNIEnv* env, jobject caller_activity)
{

}

void OpenCL_Benchmark::OnPause()
{

}

void OpenCL_Benchmark::OnDestroy()
{

}

void OpenCL_Benchmark::SetNativeWindow(ANativeWindow* nativeWindow)
{
    // Save native window
    m_native_window = nativeWindow;

    // Here we set the buffer to use RGBA_8888 as default might be; RGB_565
    if (ANativeWindow_lock(m_native_window, &m_native_buffer, NULL) == 0) {
        ANativeWindow_setBuffersGeometry(m_native_window, m_native_buffer.width, m_native_buffer.height, AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);

        // while we have the buffer size this is a good time to allocate memory to copy to it later
        m_frame_buffer = (pixel*)malloc(sizeof(pixel) * 4 * m_native_buffer.width * m_native_buffer.height);
        if (m_frame_buffer == NULL) { LOGE("Failed to allocate frame_buffer"); exit(1); }

        // save buffer info while initializing as well
        m_frame_height = m_native_buffer.height;
        m_frame_width = m_native_buffer.width;
        m_frame_stride = m_native_buffer.stride;

        ANativeWindow_unlockAndPost(m_native_window);
    }
}

double OpenCL_Benchmark::runOpenCL()
{
    // Length of vectors
    unsigned int n = 100000;

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
    size_t bytes = n*sizeof(double);

    // Allocate memory for each vector on host
    h_a = (double*)malloc(bytes);
    h_b = (double*)malloc(bytes);
    h_c = (double*)malloc(bytes);

    // Initialize vectors on host
    int i;
    for( i = 0; i < n; i++ )
    {
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
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
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
                      bytes, h_c, 0, NULL, NULL );

    //Sum up vector c and print result divided by n, this should equal 0 within error
    double sum = 0;
    for(i=0; i<n; i++)
    sum += h_c[i];
    LOGI("OpenCL final result: %f\n", sum/(double)n);

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

    // turn green for test
    pixel test = {0x00, 0xff, 0x00, 0xff};
    for (int i = 0; i < m_frame_width * m_frame_height; i++) {
        memcpy(&m_frame_buffer[i], &test, sizeof(pixel));
    }

    if (ANativeWindow_lock(m_native_window, &m_native_buffer, NULL) == 0) {
        memcpy(m_native_buffer.bits, m_frame_buffer, m_native_buffer.width * m_native_buffer.height * sizeof(pixel));
        ANativeWindow_unlockAndPost(m_native_window);
    }

    return sum;
}