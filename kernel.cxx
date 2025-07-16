#include "kernel.hpp"

//
void launcher(DATATYPE* __restrict__ d_A, DATATYPE* __restrict__ d_B, DATATYPE* __restrict__ d_res, size_t N, sycl::queue queue)
{
    queue.submit([&](sycl::handler &h) {
        h.single_task([=]() [[intel::kernel_args_restrict]] {
            // Start of kernel

            #pragma unroll UNROLL
            for (size_t i = 0; i < N; ++i) {
                const DATATYPE a = d_A[i];
                const DATATYPE b = d_B[i];
                d_res[i]         = a + b;
            }

            // End of kernel
        });
    }).wait();
}
