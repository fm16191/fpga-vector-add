#/bin/bash

for node in $(pbsnodes | grep stratix -B4 | grep s001); do
qsub -l nodes=${node}:ppn=2 -d . scripts/test_node_stratix.sh
done
