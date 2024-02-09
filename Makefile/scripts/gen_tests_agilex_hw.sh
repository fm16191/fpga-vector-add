#!/bin/bash

# 2022.3.1 -> nope
for version in $(echo "2023.2.0.1 2023.0.1 2023.2.0.2_s 2023.0 2023.2.0 2023.1.0.4 2024.0.2.1 2023.1.1_cmake 2024.0.2 2023.1.1 2024.0 2023.1.2 test" | xargs); do

fn="test_run_agilex_hw_${version}.sh"
rm -f $fn
touch $fn && chmod +x $fn

cat <<EOT | tee -a $fn > /dev/null
#!/bin/bash

#PBS -l nodes=1:fpga:fpga_runtime:agilex:ppn=2
#PBS -d .

oneapi_version=$version

source /glob/development-tools/versions/oneapi/\$oneapi_version/oneapi/setvars.sh --force

TARGET=build-agilex-\$oneapi_version
BOARD_NAME=de10_agilex:B2E2_8GBx4

# prepare env
mkdir -p \$TARGET
cp -r Makefile.fpga src \$TARGET
cd \$TARGET

# sed -i "s/\(\-Xsboard\).*$/\1=\${BOARD_NAME}/g" Makefile.fpga

echo
echo start: \$(date "+%y/%m/%d %H:%M:%S.%3N")
echo

make -f Makefile.fpga clean
make -f Makefile.fpga fpga_emu && echo "EMU OK" || echo "EMU NOPE"
make -f Makefile.fpga report && echo "REPORT OK" || echo "REPORT NOPE"
(make -f Makefile.fpga hw && echo "HW OK" || echo "HW NOPE") && (make -f Makefile.fpga run_hw && echo "RUN_HW OK" || echo "RUN_HW NOPE")

echo
echo stop: \$(date "+%y/%m/%d %H:%M:%S.%3N")
echo
EOT

done