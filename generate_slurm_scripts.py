q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3, 4]
q2_c = [4, 5, 5.1, 5.2, 5.25, 5.3, 5.33, 5.333, 5.34, 5.345, 5.4, 5.5, 5.6, 5.8, 6, 7]

print(f"q3: {len(q3_c)}, q2: {len(q2_c)}")

for c in q3_c:
    c_str = str(c)
    content = f"#!/bin/bash\n#SBATCH --nodes=1\n#SBATCH --ntasks=1\n#SBATCH --cpus-per-task=4\n#SBATCH --time=200:00:00\n#SBATCH--output=slurm/R-%x.%j.out\n\n#SBATCH--error=slurm/R-%x.%j.err\nexport OMP_NUM_THREADS=4\nexport OMP_PLACES=cores\n\nexport OMP_PROC_BIND=spread\nbash server_run.sh q3_c{c_str}\n"
    with open(f"slurm/q3_c{c}.slurm", "w") as f:
        f.write(content)

for c in q2_c:
    c_str = str(c)
    content = f"#!/bin/bash\n#SBATCH --nodes=1\n#SBATCH --ntasks=1\n#SBATCH --cpus-per-task=4\n#SBATCH --time=200:00:00\n#SBATCH--output=slurm/R-%x.%j.out\n\n#SBATCH--error=slurm/R-%x.%j.err\nexport OMP_NUM_THREADS=4\nexport OMP_PLACES=cores\n\nexport OMP_PROC_BIND=spread\nbash server_run.sh q2_c{c_str}\n"
    with open(f"slurm/q2_c{c}.slurm", "w") as f:
        f.write(content)
