#!/bin/bash -l
#SBATCH --cluster=wice
#SBATCH --partition=bigmem
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=72
#SBATCH --mem-per-cpu=28000M
#SBATCH --account=lp_h_pds_iiw
#SBATCH --time=10:00:00
#SBATCH --error="%x.e%A"
#SBATCH --output="%x.o%A"

source /data/leuven/303/vsc30380/slurmhooks

module purge
module load GCC/10.3.0

./quicksort
