#!/bin/bash

echo $HOSTNAME

source /glob/development-tools/versions/oneapi/2023.2.0.2_s/oneapi/setvars.sh --force > /dev/null
icpx --version

sycl-ls

echo $HOSTNAME
aocl diagnose all | grep "ICD diagnostics" 
./build-stratix-2023.2.0.2_s/vector-add-buffers.fpga &>/dev/null && echo "RUN YES" || echo "RUN NO"
