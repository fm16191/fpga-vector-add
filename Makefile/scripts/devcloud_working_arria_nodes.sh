#/bin/bash

for node in $(pbsnodes | grep arria -B4 | grep s001); do
qsub -l nodes=${node}:ppn=2 -d . scripts/test_node_arria.sh
done
