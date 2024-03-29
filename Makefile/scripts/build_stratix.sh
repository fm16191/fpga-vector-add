#!/bin/bash

#PBS -l nodes=1:fpga:fpga_runtime:stratix10:ppn=2
#PBS -d .

oneapi_version=""

source /glob/development-tools/versions/oneapi/$oneapi_version/oneapi/setvars.sh --force

echo oneAPI version : $oneapi_version
icpx --version

TARGET=build-stratix
BOARD_NAME=intel_s10gx_pac:pac_s10

# prepare env
mkdir -p $TARGET
cp -r Makefile.fpga src $TARGET
cd $TARGET

# sed -i "s/\(\-Xshardware\).*$/\1 -Xsboard=${BOARD_NAME}/g" Makefile.fpga

echo
echo start: $(date "+%y/%m/%d %H:%M:%S.%3N")
echo

make -f Makefile.fpga clean
make -f Makefile.fpga fpga_emu && echo "EMU OK" || echo "EMU NOPE"
make -f Makefile.fpga report && echo "REPORT OK" || echo "REPORT NOPE"
(make -f Makefile.fpga hw && echo "HW OK" || echo "HW NOPE") && (make -f Makefile.fpga run_hw && echo "RUN_HW OK" || echo "RUN_HW NOPE")

echo
echo stop: $(date "+%y/%m/%d %H:%M:%S.%3N")
echo
