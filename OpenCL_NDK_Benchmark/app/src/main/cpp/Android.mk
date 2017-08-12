LOCAL_PATH := $(call my-dir)
LIBS_PATH  := $(call my-dir)/../../../libs

include $(CLEAR_VARS)
LOCAL_MODULE     := libopencl_benchmark
LOCAL_CFLAGS     := -Werror -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/opencl12
LOCAL_SRC_FILES  := native-lib.cpp \
                    OpenCL_Benchmark.cpp
LOCAL_LDLIBS     := -llog $(LIBS_PATH)/libOpenCL.so
include $(BUILD_SHARED_LIBRARY)
