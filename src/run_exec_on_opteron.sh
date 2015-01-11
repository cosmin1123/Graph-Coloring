#!/bin/bash

# Ii dau 16 noduri in caz ca vrei sa rulezi mpi-ul pe un numar
# mai mare ca 8.

qsub -q ibm-opteron.q -pe openmpi 16 -cwd << EOF
module load compilers/gnu-4.7.0
module load libraries/openmpi-1.6-gcc-4.6.3
./timeScript.sh
EOF

