# Testing FPGA environments - vector-add case

This repository is a package of vector-add tests designed for testing compilation using oneAPI's model, targetting FPGA devices.

The source code are from oneAPI-samples, but all additional scripts are from me.

***Testing on Intel's devcloud ?***

TDLR : [devcloud-summary](#devcloud-summary)

# Testing on Intel's devcloud

The idea here is to generate a custom vector-add environment for each oneAPI version, and try it to see what's working and what's not in compiling design for emulator, for report, for hardware, and running hardware design.

One can get all oneAPI version available on Intel's devcloud :
```bash
ls /glob/development-tools/versions/oneapi/*/oneapi/setvars.sh | sed -n 's/.*oneapi\/\(.*\)\/oneapi.*/\1/p'
```

Then iterating over each oneAPI version, a new script file is created that will create a new test-folder environment, load the specified oneAPI version, then try compiling the emulator, the report, the hardware and executing hardware if sucessfuly compiled.

Script generators are located as `scripts/gen_tests_<device>_hw.sh`.

1. Generate scripts
```bash
cd scripts
./gen_tests_agilex_hw.sh
cd ..
```

2. Run all scripts. Must be in the root folder, not the script one.
```bash
for f in scripts/test_run_agilex*; do qsub $f; done
```

# Now, the results

## <a name="devcloud-summary"></a> Quick summary

- compiling for **Arria10**

- compiling for **Stratix10**

- compiling for **Agilex** \
    `source /glob/development-tools/versions/oneapi/2023.2.0.1/oneapi/setvars.sh --force` \
    or `source /glob/development-tools/versions/oneapi/2024.0.2/oneapi/setvars.sh --force`

## for Arria10
| oneapi_version | emulator | report | hardware | run_hardware |
|---|---|---|---|---|
| 2022.3.1 | NO | - | - | - |
| 2023.0.1 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.0 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.0.4 | YES | YES | NO | - |
| 2023.1.1_cmake | YES | YES | NO | - |
| 2023.1.1 | NO | YES | NO | - |
| 2023.1.2 | NO | YES | NO | - |
| 2023.2.0 | NO | YES | NO | - |
| 2023.2.0.1 | NO | YES | NO | - |
| 2023.2.0.2_s |  |  |  |  |
| 2024.0.2.1 |  |  |  |  |
| 2024.0.2 |  |  |  |  |
| 2024.0 |  |  |  |  |
| test |  |  |  |  |

## for Stratix10
| oneapi_version | emulator | report | hardware | run_hardware |
|---|---|---|---|---|
| 2022.3.1 | NO | NO | NO | - |
| 2023.0.1 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.0 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.0.4 | YES | YES | YES | NO -42 sycl::_V1::runtime_error |
| 2023.1.1_cmake |  |  |  |  |
| 2023.1.1 |  |  |  |  |
| 2023.1.2 |  |  |  |  |
| 2023.2.0 |  |  |  |  |
| 2023.2.0.1 |  |  |  |  |
| 2023.2.0.2_s |  |  |  |  |
| 2024.0.2.1 |  |  |  |  |
| 2024.0.2 |  |  |  |  |
| 2024.0 |  |  |  |  |
| test |  |  |  |  |

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
| 2024.0.2.1 |  |  |  |  |
| 2024.0.2 | YES | YES | YES | YES |
| 2024.0 |  |  |  |  |
| test |  |  |  |  |
