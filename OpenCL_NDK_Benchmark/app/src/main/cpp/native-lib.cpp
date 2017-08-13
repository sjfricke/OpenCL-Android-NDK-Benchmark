#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

#include <string>

#include "OpenCL_Benchmark.h"

static OpenCL_Benchmark app;

clock_t start_t, end_t;
double  total_t;

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM* vm, void*) {
    // We need to store a reference to the Java VM so that we can call back
    app.SetJavaVM(vm);
    return JNI_VERSION_1_6;
}
JNIEXPORT void JNICALL
Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_onCreateJNI(
        JNIEnv *env, jobject clazz, jobject j_asset_manager) {
    app.SetAssetManager(AAssetManager_fromJava(env, j_asset_manager));
}

JNIEXPORT jstring JNICALL
Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_startTest(
        JNIEnv *env, jobject clazz) {

    // for now only reading in one file
    char image_name [] = "fish.png";
    app.LoadPng(image_name);

    start_t = clock();
    double result = app.RunOpenCL();
    end_t = clock();

    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    char return_string[100];
    snprintf(return_string, sizeof(return_string), "Results: %f seconds", total_t);
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