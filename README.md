# Testing FPGA environments - vector-add case

This repository contains vector-add test cases in cmake and makefile, to test different oneAPI compilation environments targeting different FPGA devices.

The source code is from oneAPI-samples, with additional scripts added by me.

# Testing on Intel's devcloud

The goal here is to create a specific vector-add environment for each oneAPI version and test it to figure out what works and what doesn't when compiling the design for emulator, for report, for hardware, and running the hardware design.

Two things were noticed to be important: the version of the oneAPI environment being used and the node on which you are running. Certain nodes may not be properly configured, and some oneAPI environments may not function correctly for a specific device target.

---
**TLDR** : find the results for 
- [working oneAPI versions](#devcloud-summary)
- [working PBS nodes](#devcloud-nodes-summary)

--- 

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

After testing all available oneAPI environments, we can see that those work.

## <a name="devcloud-summary"></a> Quick summary

The tables below show the success state by device type and oneAPI version for each target (emulator, report, hardware, and run_hardware).

## for Arria10
| oneapi_version | emulator | report | hardware | run_hardware |
|---|---|---|---|---|
| 2022.3.1 | NO | NO | NO | - |
| 2023.0.1 | YES | NO | YES | YES |
| 2023.0 | YES | NO | YES | YES |
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
| 2023.2.0 | YES | YES | YES | YES |
| 2023.2.0.1 | YES | YES | YES | YES |
| 2023.2.0.2_s | YES | YES | YES | YES |
| 2024.0.2.1 | YES | NO | YES | YES |
| 2024.0.2 | YES | NO | YES | - |
| 2024.0 | YES | NO | YES | YES |
| test | YES | NO | YES | - |

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
| 2024.0.2.1 | YES | YES | NO | - |
| 2024.0.2 | YES | YES | YES | YES |
| 2024.0 | YES | YES | YES | YES |
| test | YES | YES | NO | - |

Is suggested to run `source /glob/development-tools/versions/oneapi/<oneAPI_version>/oneapi/setvars.sh --force` in your PBS script to ensure a valid working oneAPI compilation environment

## Reports failing

All failing reports generates the `static_reports_creation_log.temp` file, which content is for each and everyone of them :
```text
/glob/development-tools/versions/oneapi/${oneapi_version}/oneapi/compiler/${oneapi_version}/opt/oclfpga/share/lib/reports/standalone_report/fpga_report: error while loading shared libraries: libnss3.so: cannot open shared object file: No such file or directory
```

## Investigating the `Native API failed. Native API returns: -42 (PI_ERROR_INVALID_BINARY)` error

***TLDR if testing on Intel's devcloud :*** *Some nodes are not properly configured, find all properly configured nodes [here](#devcloud-nodes-summary)*

---

On Internet, this errors doesn't yield much information.
A [post from the intel community forums](https://community.intel.com/t5/Intel-DevCloud/Native-API-failed-Native-API-returns-42-PI-ERROR-INVALID-BINARY/m-p/1492937?profile.language=zh-TW) indicate about an incorrect correct board variant initialised, but no more information.

So I set a small experiment : 

I had already managed to compile the vector-add for Arria10 using oneAPI version 2024.0.2.1, so I already had the executable at `./build-arria-2024.0.2.1/vector-add-buffers.fpga`

I then started two terminals, connected both to the devcloud, sourced oneAPI version 2024.0.2.1 and then qsub an interactive arria10 partition.

```bash
ssh devcloud
cd fpga-vector-add/Makefile
source /glob/development-tools/versions/oneapi/2024.0.2.1/oneapi/setvars.sh --force
./build-arria-2024.0.2.1/vector-add-buffers.fpga
```

- first interactive session gives me the `s001-n081` node
```txt
@s001-n081 Makefile ❯ ./build-arria-2024.0.2.1/vector-add-buffers.fpga
Running on device: Intel(R) Xeon(R) Gold 6128 CPU @ 3.40GHz
Vector size: 10000
An exception is caught for vector add.
terminate called after throwing an instance of 'sycl::_V1::runtime_error'
  what():  Native API failed. Native API returns: -42 (PI_ERROR_INVALID_BINARY) -42 (PI_ERROR_INVALID_BINARY)
Aborted
```

- second interactive session gives me the `s001-n083` node
```txt
@s001-n083 Makefile ❯ ./build-arria-2024.0.2.1/vector-add-buffers.fpga
Running on device: pac_a10 : Intel PAC Platform (pac_ee00000)
Vector size: 10000
[0]: 0 + 0 = 0
[1]: 1 + 1 = 2
[2]: 2 + 2 = 4
...
[9999]: 9999 + 9999 = 19998
Vector add successfully completed on device.
```

Has the executable been compiled using node `s001-n083`? Is it node-dependent? What are the differences between the two nodes?

Let's compare the two environments to see if we can identify a cause.

- first node `s001-n081`:
```txt
@s001-n081 Makefile ❯ aoc -list-boards
...
  pac_a10 (default)
     Board Package: /opt/intel/oneapi/intel_a10gx_pac
...
```

- second node `s001-n083`:
```txt
@s001-n083 Makefile ❯ aoc -list-boards
...
  pac_a10 (default)
     Board Package: /opt/intel_2023.1/oneapi/intel_a10gx_pac
...
```

a `diff -r /opt/intel/oneapi/intel_a10gx_pac /opt/intel_2023.1/oneapi/intel_a10gx_pac` reveals nothing. There are the same folders

- first node `s001-n081`:
```txt
@s001-n081 Makefile ❯ sycl-ls
[opencl:cpu:0] Intel(R) OpenCL, Intel(R) Xeon(R) Gold 6128 CPU @ 3.40GHz OpenCL 3.0 (Build 0) [2023.16.11.0.22_160000]
```

- first node `s001-n083`:
```txt
@s001-n081 Makefile ❯ sycl-ls
[opencl:cpu:0] Intel(R) OpenCL, Intel(R) Xeon(R) Gold 6128 CPU @ 3.40GHz OpenCL 3.0 (Build 0) [2023.16.11.0.22_160000]
[opencl:acc:1] Intel(R) FPGA SDK for OpenCL(TM), pac_a10 : Intel PAC Platform (pac_ee00000) OpenCL 1.0 Intel(R) FPGA SDK for OpenCL(TM), Version 2024.0 [2024.0]
[opencl:acc:2] Intel(R) FPGA Emulation Platform for OpenCL(TM), Intel(R) FPGA Emulation Device OpenCL 1.2  [2023.16.11.0.22_160000]
```

There is definitely something going on here. The first node doesn't seem to be configured well.

So I opened a new terminal and did the same steps. Got node `s001-n084`.
```txt
@s001-n084 Makefile ❯ ./build-arria-2024.0.2.1/vector-add-buffers.fpga
Running on device: pac_a10 : Intel PAC Platform (pac_ee00000)
Vector size: 10000
[0]: 0 + 0 = 0
[1]: 1 + 1 = 2
[2]: 2 + 2 = 4
...
[9999]: 9999 + 9999 = 19998
Vector add successfully completed on device.

@s001-n084 Makefile ❯ sycl-ls
[opencl:cpu:0] Intel(R) OpenCL, Intel(R) Xeon(R) Gold 6128 CPU @ 3.40GHz OpenCL 3.0 (Build 0) [2023.16.11.0.22_160000]
[opencl:acc:1] Intel(R) FPGA SDK for OpenCL(TM), pac_a10 : Intel PAC Platform (pac_ee00000) OpenCL 1.0 Intel(R) FPGA SDK for OpenCL(TM), Version 2024.0 [2024.0]
[opencl:acc:2] Intel(R) FPGA Emulation Platform for OpenCL(TM), Intel(R) FPGA Emulation Device OpenCL 1.2  [2023.16.11.0.22_160000]

@s001-n084 Makefile ❯ aoc -list-boards
...
  pac_a10 (default)
     Board Package: /opt/intel/oneapi/intel_a10gx_pac
...
```

Nodes `s001-n081`, `s001-n083` and `s001-n084` show the exact same properties when using `pbsnodes`. So it shouldn't be this...


Looking at the `aocl diagnose all` report, the non-working node (`s001-n081`) reports :
```txt
WARNING: Skipping ICD validation since there is no ICD entry at this location
...
ERROR: No ICD entry and all files specified by OCL_ICD_FILENAMES are invalid
...
ICD diagnostics FAILED
```

while the working ones report `ICD diagnostics PASSED`

Tho, both non-working and working node reports `DIAGNOSTIC_PASSED`

## Working nodes

Knowing that some environments are not properly configured, I set up some scripts to check which nodes are working. 

I used those commands to get the nodelist, then run a small script that sources the oneAPI environment, print some environement stuff then tries executing the fpga design.

```bash
pbsnodes | grep arria -B4 | grep s001 # Arria10 nodes
pbsnodes | grep stratix -B4 | grep s001 # Stratix10 nodes
pbsnodes | grep agilex -B4 | grep s006 # Agilex nodes
```

The scripts can be found at `scripts/devcloud_working_<device>_nodes.sh` where device is arria, stratix or agilex. These scripts will run for the `test_node_<device>.sh` script for all nodes, which will tell if the node is configured correctly or not. 

## <a name="devcloud-nodes-summary"></a> Here are the results

### Arria10 nodes

| Arria10 nodes | Working state |
| --- | --- |
| s001-n081 | NO |
| s001-n082 |  |
| s001-n083 | YES |
| s001-n084 | YES |
| s001-n085 |  |
| s001-n086 | NO |
| s001-n087 | YES |
| s001-n088 |  |
| s001-n089 | NO |
| s001-n090 |  |
| s001-n091 |  |
| s001-n092 |  |

### Stratix10 nodes

| Stratix10 nodes | Working state |
| --- | --- |
| s001-n010 |  |
| s001-n142 | NO |
| s001-n143 | YES |
| s001-n144 | NO |

### Agilex nodes

| Agilex nodes | Working state |
| --- | --- |
| s006-n001 |  |
| s006-n002 | YES |
| s006-n003 | YES |


Bad news it that once the working nodes were spotted, attempting to re-compile the vector-add using the oneAPI versions that result int the `-42 sycl::_V1::runtime_error` error, still allow for a working compilation for hardware design, it still couldn't be runned on the node, and returns the same error.

<!--
### other experiments

Looking at `lshw`, same goes on. Even tho I don't have root access, the hardware reported seems to be the same, with changes in MAC and IP adresses.

Looking at `env`, few changes are noticeable between `s001-n081` (not working), `s001-n083` (working) nodes, while `s001-n083` and `s001-n084` have exact same env except for hostname and jobID (of course)

`s001-n083` node has the following env properties more than `s001-n081`:
```txt
mypc ❯ diff env_s001-n081 env_s001-n083 | grep ">"
> LIBVA_DRIVER_NAME=iHD
> LIBVA_DRIVERS_PATH=/opt/intel/mediasdk/lib64
> MODULES_CMD=/usr/lib/x86_64-linux-gnu/modulecmd.tcl
> ENV=/usr/share/modules/init/profile.sh
> BASH_ENV=/usr/share/modules/init/bash
> LOADEDMODULES=
> MFX_HOME=/opt/intel/mediasdk
> MODULEPATH=/etc/environment-modules/modules:/usr/share/modules/versions:/usr/share/modules/$MODULE_VERSION/modulefiles:/glob/module-files/development-tools:/glob/module-files/intel-oneapi-components:/glob/module-files/intel-oneapi
> MODULEPATH_modshare=/glob/module-files/intel-oneapi:1:/glob/module-files/development-tools:1:/usr/share/modules/$MODULE_VERSION/modulefiles:1:/etc/environment-modules/modules:1:/glob/module-files/intel-oneapi-components:1:/usr/share/modules/versions:1
> MODULESHOME=/usr/share/modules
> BASH_FUNC_module%%=() {  _moduleraw "$*" 2>&1
> }
> BASH_FUNC_switchml%%=() {  typeset swfound=1;
>  if [ "${MODULES_USE_COMPAT_VERSION:-0}" = '1' ]; then
>  typeset swname='main';
>  if [ -e /usr/lib/x86_64-linux-gnu/modulecmd.tcl ]; then
>  typeset swfound=0;
>  unset MODULES_USE_COMPAT_VERSION;
>  fi;
>  else
>  typeset swname='compatibility';
>  if [ -e /usr/lib/x86_64-linux-gnu/modulecmd-compat ]; then
>  typeset swfound=0;
>  MODULES_USE_COMPAT_VERSION=1;
>  export MODULES_USE_COMPAT_VERSION;
>  fi;
>  fi;
>  if [ $swfound -eq 0 ]; then
>  echo "Switching to Modules $swname version";
>  source /usr/share/modules/init/bash;
>  else
>  echo "Cannot switch to Modules $swname version, command not found";
>  return 1;
>  fi
> }
> BASH_FUNC__moduleraw%%=() {  unset _mlre _mlIFS _mlshdbg;
>  if [ "${MODULES_SILENT_SHELL_DEBUG:-0}" = '1' ]; then
>  case "$-" in 
>  *v*x*)
>  set +vx;
>  _mlshdbg='vx'
>  ;;
[...]
>  set -$_mlshdbg;
>  fi;
>  unset _mlre _mlv _mlrv _mlIFS _mlshdbg;
>  return $_mlstatus
> }
```

Those are specific env differences that the working node `s001-n083` has setted up, but `s001-n081` has not.

I naively tried to set them manually, but it didn't help. -->


# Credits

Source code from Intel's [oneAPI-samples](https://github.com/oneapi-src/oneAPI-samples/tree/master/DirectProgramming/C%2B%2BSYCL/DenseLinearAlgebra/vector-add)

Scripts are mine