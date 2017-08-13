LOCAL_PATH := $(call my-dir)
LIBS_PATH  := $(call my-dir)/../../../libs

include $(CLEAR_VARS)
LOCAL_MODULE     := libopencl_benchmark
LOCAL_STATIC_LIBRARIES := png

LOCAL_CFLAGS     := -Werror -std=c++11

LOCAL_C_INCLUDES := $(LIBS_PATH)/CL/opencl12
LOCAL_C_INCLUDES += $(LIBS_PATH)/libpng/include/

LOCAL_SRC_FILES  := native-lib.cpp \
                    OpenCL_Benchmark.cpp \
                    Vector_Add.cpp \
                    Gaussian_Blur.cpp

LOCAL_LDLIBS     := -llog -landroid $(LIBS_PATH)/libOpenCL.so
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LIBS_PATH))
$(call import-module, libpng)