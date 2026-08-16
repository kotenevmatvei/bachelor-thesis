
q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3]

for c in q3_c:
    c_str = str(c)
    content = f"#!/bin/bash\n#SBATCH --nodes=1\n#SBATCH --ntasks=1\n#SBATCH --cpus-per-task=5\n#SBATCH --time=8:00:00\n#SBATCH--output=slurm/R-%x.%j.out\n\n#SBATCH--error=slurm/R-%x.%j.err\nexport OMP_NUM_THREADS=4\nexport OMP_PLACES=cores\n\nexport OMP_PROC_BIND=spread\nbash server_run.sh q3_c{c_str}\n"
    with open(f"slurm/q3_c{c}.slurm", "w") as f:
        f.write(content)

