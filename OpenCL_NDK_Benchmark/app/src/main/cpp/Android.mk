LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libopencl_benchmark
LOCAL_CFLAGS    := -Werror -std=c++11
LOCAL_SRC_FILES := native-lib.cpp \
                   OpenCL_Benchmark.cpp
LOCAL_LDLIBS    := -llog -L$(SYSROOT)/usr/lib
include $(BUILD_SHARED_LIBRARY)
