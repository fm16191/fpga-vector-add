#ifndef KERNEL_H_
#define KERNEL_H_

#ifndef FPGA_HARDWARE
#define FPGA_HARDWARE 0
#endif
#ifndef FPGA_EMULATOR
#define FPGA_EMULATOR 0
#endif
#ifndef FPGA_SIMULATOR
#define FPGA_SIMULATOR 0
#endif

#include <sycl/sycl.hpp>
#if FPGA_HARDWARE || FPGA_EMULATOR || FPGA_SIMULATOR
#include <sycl/ext/intel/fpga_extensions.hpp>
#endif

#ifndef DATATYPE
#define DATATYPE double
#endif

#ifndef UNROLL
#define UNROLL 0
#endif

void launcher(DATATYPE *__restrict__ d_A, DATATYPE *__restrict__ d_B, DATATYPE *__restrict__ d_res, size_t N,
              sycl::queue queue);

#endif  // KERNEL_H_
