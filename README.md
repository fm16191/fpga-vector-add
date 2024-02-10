# Testing FPGA environments - vector-add case

This repository contains vector-add test cases in cmake and makefile, to test different oneAPI compilation environments targeting different FPGA devices.

The source code is from oneAPI-samples, with additional scripts added by me.

***Testing on Intel's devcloud ?***

TDLR : [devcloud-summary](#devcloud-summary)

# Testing on Intel's devcloud

The goal here is to create a specific vector-add environment for each oneAPI version and test it to figure out what works and what doesn't when compiling the design for emulator, for report, for hardware, and running the hardware design.

All available versions of oneAPI can be obtained from Intel's DevCloud using :
```bash
ls /glob/development-tools/versions/oneapi/*/oneapi/setvars.sh | sed -n 's/.*oneapi\/\(.*\)\/oneapi.*/\1/p'
```

Iterating over each oneAPI version, a new script file is created to generate a test-folder environment, load the specified oneAPI version, and attempt to compile the emulator, report, and hardware. If hardware compilation is successful, the design is executed.

The script generators can be found as `scripts/gen_tests_<device>_hw.sh`.

1. Generate scripts
```bash
cd scripts
./gen_tests_agilex_hw.sh
cd ..
```

1. Run all scripts from the root folder, not the scripts folder.
```bash
for f in scripts/test_run_agilex*; do qsub $f; done
```

# Now, the results

## <a name="devcloud-summary"></a> Quick summary

- compiling for **Arria10** \
    `source /glob/development-tools/versions/oneapi/2023.2.0.2_s/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0.2.1/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0.2/oneapi/setvars.sh --force`

- compiling for **Stratix10** \
    `source /glob/development-tools/versions/oneapi/2023.1.1/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2023.2.0.2_s/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0.2.1/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0/oneapi/setvars.sh --force`

- compiling for **Agilex** \
    `source /glob/development-tools/versions/oneapi/2023.2.0.1/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0.2/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0/oneapi/setvars.sh --force`

## for Arria10
| oneapi_version | emulator | report | hardware | run_hardware |
|---|---|---|---|---|
| 2022.3.1 | NO | NO | NO | - |
| 2023.0.1 | YES | NO | YES | NO -42 sycl::_V1::runtime_error |
| 2023.0 | YES | NO | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.0.4 | YES | NO | NO | - |
| 2023.1.1_cmake | YES | NO | NO | - |
| 2023.1.1 | NO | NO | NO | - |
| 2023.1.2 | NO | NO | NO | - |
| 2023.2.0 | NO | NO | NO | - |
| 2023.2.0.1 | NO | NO | NO | - |
| 2023.2.0.2_s | YES | YES | YES | YES |
| 2024.0.2.1 | YES | YES | YES | YES |
| 2024.0.2 | YES | NO | YES | YES |
| 2024.0 | NO | NO | NO | - |
| test | YES | NO | NO | - |

## for Stratix10
| oneapi_version | emulator | report | hardware | run_hardware |
|---|---|---|---|---|
| 2022.3.1 | NO | NO | NO | - |
| 2023.0.1 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.0 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.0.4 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.1_cmake | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.1 | YES | YES | YES | YES |
| 2023.1.2 | YES | YES | NO | - |
| 2023.2.0 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.2.0.1 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.2.0.2_s | YES | YES | YES | YES |
| 2024.0.2.1 | YES | NO | YES | YES |
| 2024.0.2 | YES | NO | YES | NO |
| 2024.0 | YES | NO | YES | YES |
| test | YES | NO | YES | NO |

## for Agilex
| oneapi_version | emulator | report | hardware | run_hardware |
|---|---|---|---|---|
| 2022.3.1 | NO | NO | NO | - |
| 2023.0.1 | YES | YES | NO | - |
| 2023.0 | YES | YES | NO | - |
| 2023.1.0.4 | YES | YES | NO | - |
| 2023.1.1 | YES | YES | NO | - |
| 2023.1.1_cmake | YES | YES | NO | - |
| 2023.1.2 | YES | YES | NO | - |
| 2023.2.0.2_s | YES | YES | NO | - |
| 2023.2.0 | YES | YES | NO | - |
| 2023.2.0.1 | YES | YES | YES | YES |
| 2024.0.2.1 | YES | YES | NO | NO |
| 2024.0.2 | YES | YES | YES | YES |
| 2024.0 | YES | YES | YES | YES |
| test | YES | YES | NO | - |



# Credits

Source code from Intel's [oneAPI-samples](https://github.com/oneapi-src/oneAPI-samples/tree/master/DirectProgramming/C%2B%2BSYCL/DenseLinearAlgebra/vector-add)

Scripts are mine