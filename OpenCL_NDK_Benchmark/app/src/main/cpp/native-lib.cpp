#include <jni.h>
#include <android/native_window_jni.h>

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
  double result = app.runOpenCL();
  char return_string[100];
  snprintf(return_string, sizeof(return_string), "Time: %f seconds", result);
  return env->NewStringUTF(return_string);
}

// set the surface
void Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface)
{
  // obtain a native window from a Java surface
  app.SetNativeWindow( ANativeWindow_fromSurface(env, surface) );
}


#ifdef __cplusplus
}
#endif