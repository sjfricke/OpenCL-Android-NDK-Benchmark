#ifndef OPENCL_NDK_BENCHMARK_OPENCL_BENCHMARK_H
#define OPENCL_NDK_BENCHMARK_OPENCL_BENCHMARK_H

#include <android/log.h>
#include <jni.h>

#include <cstdlib>
#include <unistd.h>

#include <string>
#include <vector>

// used to get logcat outputs which can be regex filtered by the LOG_TAG we give
// So in Logcat you can filter this example by putting OpenCL_Benchmark
#define LOG_TAG "OpenCL_Benchmark"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class OpenCL_Benchmark {
public:

  OpenCL_Benchmark() {}

  OpenCL_Benchmark(const OpenCL_Benchmark& other) = delete;

  OpenCL_Benchmark& operator=(const OpenCL_Benchmark& other) = delete;

  ~OpenCL_Benchmark() {}

  // Lets us know when app has started passing in VM info
  void OnCreate(JNIEnv* env, jobject caller_activity);

  // Disconnect from service
  void OnPause();

  // Cleanup
  void OnDestroy();

  // Cache the Java VM used from the Java layer.
  void SetJavaVM(JavaVM* java_vm) { java_vm_ = java_vm; }

private:

  // Cached Java VM, caller activity object
  JavaVM* java_vm_;
  jobject calling_activity_obj_;
  jmethodID on_demand_method_;

};

#endif //OPENCL_NDK_BENCHMARK_OPENCL_BENCHMARK_H
