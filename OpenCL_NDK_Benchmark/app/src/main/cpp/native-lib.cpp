#include <jni.h>
#include <string>

#include "OpenCL_Benchmark.h"

static OpenCL_Benchmark app;

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM* vm, void*) {
  // We need to store a reference to the Java VM so that we can call back
  app.SetJavaVM(vm);
  return JNI_VERSION_1_6;
}

JNIEXPORT jstring JNICALL
Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_stringFromJNI(
    JNIEnv *env,  jobject) {
  std::string hello = "OpenCL NDK Bencmark";
  return env->NewStringUTF(hello.c_str());
}

#ifdef __cplusplus
}
#endif