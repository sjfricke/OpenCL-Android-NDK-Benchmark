#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

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
JNIEXPORT void JNICALL
Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_onCreateJNI(
        JNIEnv *env, jobject clazz, jobject activity, jobject j_asset_manager) {
    app.OnCreate(env, activity);
    app.SetAssetManager(AAssetManager_fromJava(env, j_asset_manager));
}

JNIEXPORT jstring JNICALL
Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_startTest(
        JNIEnv *env, jobject clazz) {

    double time_Vector_Add, time_Gaussian_Blur;

    // Run the test
    time_Vector_Add = app.Vector_Add();
    time_Gaussian_Blur = app.Gaussian_Blur();

    char return_string[512];
    snprintf(return_string, sizeof(return_string),
             "Vector Add: %f seconds\n "
             "Gaussian Blur: %f seconds\n",
             time_Vector_Add, time_Gaussian_Blur);
    return env->NewStringUTF(return_string);
}

// Alot of stuff depends on the m_frame_buffer being loaded
// this is done in SetNativeWindow
void Java_com_spencerfricke_opencl_1ndk_1benchmark_MainActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface)
{
    // obtain a native window from a Java surface
    app.SetNativeWindow( ANativeWindow_fromSurface(env, surface) );

    // for now only reading in one file
    char image_name [] = "fish.png";
    app.LoadPng(image_name);
}


#ifdef __cplusplus
}
#endif