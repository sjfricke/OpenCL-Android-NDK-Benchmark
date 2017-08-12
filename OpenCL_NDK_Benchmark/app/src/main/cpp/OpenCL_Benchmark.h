#ifndef OPENCL_NDK_BENCHMARK_OPENCL_BENCHMARK_H
#define OPENCL_NDK_BENCHMARK_OPENCL_BENCHMARK_H

#include <android/log.h>
#include <android/native_window.h>
#include <jni.h>

#include <cstdlib>
#include <unistd.h>

#include <string>
#include <vector>

#include <CL/opencl.h>

// used to get logcat outputs which can be regex filtered by the LOG_TAG we give
// So in Logcat you can filter this example by putting OpenCL_Benchmark
#define LOG_TAG "OpenCL_Benchmark"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class OpenCL_Benchmark {
public:
  OpenCL_Benchmark();
  ~OpenCL_Benchmark();
  OpenCL_Benchmark(const OpenCL_Benchmark& other) = delete;
  OpenCL_Benchmark& operator=(const OpenCL_Benchmark& other) = delete;

  // Lets us know when app has started passing in VM info
  void OnCreate(JNIEnv* env, jobject caller_activity);

  // Disconnect from service
  void OnPause();

  // Cleanup
  void OnDestroy();

  // Cache the Java VM used from the Java layer.
  void SetJavaVM(JavaVM* java_vm) { m_java_vm = java_vm; }

  // sets Surface buffer reference pointer
  void SetNativeWindow(ANativeWindow* nativeWindow) { m_native_window = nativeWindow; }

  double runOpenCL();

private:

  // Cached Java VM, caller activity object
  JavaVM* m_java_vm;
  jobject m_calling_activity_obj;
  jmethodID m_demand_method;

  ANativeWindow* m_native_window;

  // Compute c = a + b.
  const char *kernel_source =
      "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
          "__kernel void vecAdd(  __global double *a,\n"
          "                       __global double *b,\n"
          "                       __global double *c,\n"
          "                       const unsigned int n)\n"
          "{\n"
          "    int id = get_global_id(0);\n"
          "    if (id < n) {\n"
          "        c[id] = a[id] + b[id];\n"
          "    }\n"
          "}\n";
};

#endif //OPENCL_NDK_BENCHMARK_OPENCL_BENCHMARK_H
