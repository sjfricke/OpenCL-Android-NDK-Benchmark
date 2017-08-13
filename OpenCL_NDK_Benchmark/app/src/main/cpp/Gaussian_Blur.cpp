#include "OpenCL_Benchmark.h"

double OpenCL_Benchmark::Gaussian_Blur()
{
    clock_t start_t, end_t;
    double  time_t;
    start_t = clock(); // Start Time!

    if (ANativeWindow_lock(m_native_window, &m_native_buffer, NULL) == 0) {
        memcpy(m_native_buffer.bits, m_frame_buffer, m_native_buffer.width * m_native_buffer.height * sizeof(pixel));
        ANativeWindow_unlockAndPost(m_native_window);
    }

    // Ends timing since rest is validation and cleanup
    end_t = clock();
    time_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    return time_t;
}