#include "OpenCL_Benchmark.h"

OpenCL_Benchmark::OpenCL_Benchmark()
{
};

OpenCL_Benchmark::~OpenCL_Benchmark()
{
    // clean up VM and callback handles
    JNIEnv* env;
    java_vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    env->DeleteGlobalRef(calling_activity_obj);
    calling_activity_obj = nullptr;
    on_png_loaded = nullptr;

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
    // Need to create an instance of the Java activity
    calling_activity_obj = env->NewGlobalRef(caller_activity);

    // Need to enter package and class to find Java class
    jclass handler_class = env->GetObjectClass(caller_activity);

    // Create function pointer to use for on_loaded callbacks
    on_png_loaded = env->GetMethodID(handler_class, "loadedPng", "()V");
}

void OpenCL_Benchmark::OnPause()
{
}

void OpenCL_Benchmark::OnDestroy()
{
}

void OpenCL_Benchmark::SetNativeWindow(ANativeWindow* native_window)
{
    // Save native window
    m_native_window = native_window;

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

void OpenCL_Benchmark::LoadPng(char* file_path)
{
    LOGI("LoadPNg");
    int width, height;
    png_byte color_type;
    png_bytep *row_pointers;

    // read in file from assent manager
    // TODO this should block in case surface loads fast and this is called before AssetManager
    AAsset* asset = AAssetManager_open(m_aasset_manager, file_path, AASSET_MODE_STREAMING);
    if (asset == NULL) {
        LOGE("Error opening asset %s", file_path); exit(1);
    }

    off_t length;
    off_t start;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    lseek(fd, start, SEEK_CUR);
    FILE* file = fdopen(fd, "rb");

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    setjmp(png_jmpbuf(png_ptr));
    png_init_io(png_ptr, file);
    png_read_info(png_ptr, info_ptr);

    width      = png_get_image_width(png_ptr, info_ptr);
    height     = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);

    int row = width * (color_type == PNG_COLOR_TYPE_RGBA ? 4 : 3);
    char* byte_data = new char[row * height];

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr,info_ptr));
    }

    if (color_type != PNG_COLOR_TYPE_RGBA) {
        LOGE("RGB not supported without alpha yet"); exit(1);
    }

    png_read_image(png_ptr, row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    // sets png image buffer to frame buffer
    // TODO read in by line, not by pixel
    for(int y = 0; y < height; y++) {
        png_bytep pRow = row_pointers[y];
        for(int x = 0; x < width; x++) {
            memcpy(&m_frame_buffer[(y * m_frame_stride) + x], &(pRow[x * 4]), 4);
        }
    }

    // clean up so just the frame_buffer remains
    delete[] row_pointers;
    delete[] byte_data;
    fclose(file);
    AAsset_close(asset);

    // Here, we notify the Java activity that we finished loading
    JNIEnv *env;
    java_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    env->CallVoidMethod(calling_activity_obj, on_png_loaded);
}