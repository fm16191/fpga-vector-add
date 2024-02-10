#!/bin/bash

echo $HOSTNAME

source /glob/development-tools/versions/oneapi/2024.0.2/oneapi/setvars.sh --force > /dev/null
icpx --version

sycl-ls

echo $HOSTNAME
aocl diagnose all | grep "ICD diagnostics" 
./build-agilex-2024.0.2/vector-add-buffers.fpga &>/dev/null && echo "RUN YES" || echo "RUN NO"
