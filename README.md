# Vector Add FPGA Example

A "Hello World" equivalent for Intel FPGA devices using Intel oneAPI (up to version 2025.0). This example demonstrates vector addition (`C[i] = A[i] + B[i]`) on FPGA hardware using SYCL programming model.

## Overview

This example performs element-wise addition of two vectors using Intel's oneAPI SYCL framework. The kernel reads from two input arrays and writes the sum to an output array. The implementation uses manual memory management with explicit `memcpy` operations for clarity and educational purposes.

## Development Process

The FPGA development workflow consists of three stages with increasing compilation time:

1. **Emulation** (seconds) to ensure functional validation
   - CPU emulation: `make cpu`
   - FPGA emulator: `make fpga_emu`

2. **Report Generation** (minutes)
   - `make report` - Generates hardware resource utilization and performance reports
   - **Requires BOARD_NAME configuration**

3. **Hardware Compilation** (hours)
   - `make fpga` - Full hardware synthesis and place-and-route
   - `make recompile_fpga` - Recompiles host code with existing kernel binary
   - **Requires BOARD_NAME configuration**

## Configuration

### Board Setup

Before generating reports or executing on FPGA hardware, you must specify your target board in the Makefile:

```makefile
BOARD_NAME := your_board_name
```

Common board names:
- `intel_a10gx_pac:pac_a10` - Intel Arria 10
- `intel_s10gx_pac:pac_s10` - Intel Stratix 10
- `ia840f:ofs_ia840f` - Intel Agilex 7 (Bittware's IA840F)
- `/path/to/IOFS_BUILD_ROOT/oneapi-asp/<folder>:<variant>`

Edit the Makefile and uncomment/modify the appropriate board configuration for your system.

## Compilation Targets

### Quick Development
```bash
# CPU execution (fastest for development)
make cpu
./vec_add.cpu

# FPGA emulator (seconds)
make fpga_emu
./vec_add.fpga_emu
```

### Performance Analysis
```bash
# Generate hardware reports
make report

# Full hardware compilation
make fpga
./vec_add.fpga
```

### Host-Only Recompilation
```bash
# Recompile only host code (seconds vs hours)
make recompile_fpga
```

## Architecture

### File Structure
- `main.cxx` - Host code with memory allocation, data transfer, and timing
- `kernel.cxx` - Device kernel implementing vector addition
- `kernel.hpp` - Header with SYCL kernel interface
- `Makefile` - Build system with multiple targets

### Memory Management
This example uses manual memory allocation with explicit `memcpy` operations:
- `malloc_device<DATATYPE>()` for device memory
- `queue.memcpy()` for host-device transfers
- No USM (Unified Shared Memory) for educational clarity

### Kernel Implementation
The kernel uses a `single_task` with optional loop unrolling:
```cpp
h.single_task([=]() [[intel::kernel_args_restrict]] {
    #pragma unroll UNROLL
    for (size_t i = 0; i < N; ++i) {
        const DATATYPE a = d_A[i];
        const DATATYPE b = d_B[i];
        d_res[i] = a + b;
    }
}); 
```

## Performance Metrics

- **II** stands for "Initiation Interval" - the number of clock cycles between starting consecutive iterations of a loop. An II of 1 means the loop can start a new iteration every clock cycle (optimal). Higher II values indicate pipeline stalls or resource conflicts.
- **Frequency**: Design clock frequency read from executable file (using `aocl info`), used for II calculations. FPGA designs run at fixed frequencies (typically 200-500 MHz) determined during synthesis, unlike CPUs that can dynamically adjust clock speeds.
- **Throughput**: Measured memory transfer rate in GB/s, reported separately for kernel (load/store) and host-device transfers

### Optimization Features
- **Loop Unrolling**: Controlled by `UNROLL` macro at compile time. Disabled by default
- **Restrict pointers**: `__restrict__` keyword prevents pointer aliasing
- **Kernel arguments restrict**: `[[intel::kernel_args_restrict]]` attribute

## Compilation Options

### Data Type
```bash
# Use float instead of double
make USE_FLOAT=1 fpga
```

### Loop Unrolling
```bash
# Enable unrolling (modify kernel.hpp)
#define UNROLL 1
```

## Key Features

1. **Separated Host/Kernel Compilation**: Kernel compiled to dynamic library, enabling fast host-only recompilation
2. **Multiple Execution Targets**: CPU, emulator, simulator, and hardware
3. **Comprehensive Timing**: Detailed breakdown of memory transfers and computation
4. **Performance Analysis**: Automatic II calculation and throughput estimation
5. **Verification**: Built-in result validation against CPU reference

## Troubleshooting

### Common Issues
- **Board not found**: Verify `BOARD_NAME` in Makefile
- **Memory insufficient**: Reduce problem size or use smaller data type
- **Compilation errors**: Check oneAPI installation and board support

### Performance Tips
- Use loop unrolling for better II
- Consider data type impact (float vs double, or even more specific data types, using ac_int / ap_float)
- Monitor memory bandwidth utilization

## Dependencies

- Intel oneAPI Base Toolkit (2024.0 or later)
- Intel oneAPI HPC Toolkit
- Intel FPGA Add-on for oneAPI
- Supported Intel FPGA board

## License

MIT License - see [LICENSE](LICENSE) file for details.

This example is provided as-is for educational purposes. The MIT License allows for maximum freedom in using, modifying, and distributing the code while providing minimal liability protection.
