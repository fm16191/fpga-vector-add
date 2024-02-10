#/bin/bash

for node in $(pbsnodes | grep agilex -B4 | grep s006); do
qsub -l nodes=${node}:ppn=2 -d . scripts/test_node_agilex.sh
done
