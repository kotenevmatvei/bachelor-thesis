import os

def generate_c_sweep():
    q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3, 4]
    q2_c = [4, 5, 5.1, 5.2, 5.25, 5.3, 5.33, 5.333, 5.34, 5.345, 5.4, 5.5, 5.6, 5.8, 6, 7]

    print(f"q3: {len(q3_c)}, q2: {len(q2_c)}")

    power_run_names = [
        "q2-3_c_sweep_symmetric_init-uniform", # already run
        "q2-3_c_sweep_symmetric_init-demixed",
        "q2-3_c_sweep_cyclic_init-uniform",
    ]

    for run_name in power_run_names:
        os.makedirs(f"slurm/{run_name}", exist_ok=True)

        for q_val in [2, 3]:
            c_list = q2_c if q_val == 2 else q3_c
            for c_val in c_list:
                content = f"""#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/{run_name}/R-%x.%j.out
#SBATCH --error=slurm/{run_name}/R-%x.%j.err

bash server_run.sh "configs/{run_name}/q{q_val}_c{c_val}.txt"
"""
                with open(f"slurm/{run_name}/q{q_val}_c{c_val}.slurm", "w") as f:
                    f.write(content)


def generate_p0_sweep():
    p_0_list = [0, 0.2, 0.4, 0.5, 0.6, 0.8, 1]

    run_name = "alpha7-8_p0_sweep_symmetric_init-demixed"

    os.makedirs(f"slurm/{run_name}", exist_ok=True)

    for alpha in [7, 8]:
        for p_0 in p_0_list:
            content = f"""#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/{run_name}/R-%x.%j.out
#SBATCH --error=slurm/{run_name}/R-%x.%j.err

bash server_run.sh "configs/{run_name}/alpha{alpha}_p0{p_0}.txt"
"""
            with open(f"slurm/{run_name}/alpha{alpha}_p0{p_0}.slurm", "w") as f:
                f.write(content)


def generate_rs_sweep_power():
    rs_list = [0, 1, 2, 4, 10, 20, 30, 40, 47, 48, 60, 61, 70]
    runs = [
        {
            "name": "q3_c3_rs-sweep_symmetric_init-uniform",
            "q": 3,
            "c": 3,
            "dependency": "symmetric",
            "init_density": "uniform",
        },
        {
            "name": "q3_c3_rs-sweep_symmetric_init-uniform",
            "q": 3,
            "c": 3,
            "dependency": "symmetric",
            "init_density": "demixed",
        },
        {
            "name": "q2_c6_rs-sweep_symmetric_init-uniform",
            "q": 2,
            "c": 6,
            "dependency": "symmetric",
            "init_density": "uniform",
        },
        {
            "name": "q2_c6_rs-sweep_symmetric_init-demixed",
            "q": 2,
            "c": 6,
            "dependency": "symmetric",
            "init_density": "demixed",
        },
        {
            "name": "q3_c3_rs-sweep_cyclic_init-uniform",
            "q": 3,
            "c": 3,
            "dependency": "cyclic",
            "init_density": "uniform",
        },
        {
            "name": "q3_c3_rs-sweep_cyclic_init-uniform",
            "q": 3,
            "c": 3,
            "dependency": "cyclic",
            "init_density": "demixed",
        },
        {
            "name": "q2_c6_rs-sweep_cyclic_init-uniform",
            "q": 2,
            "c": 6,
            "dependency": "cyclic",
            "init_density": "uniform",
        },
        {
            "name": "q2_c6_rs-sweep_cyclic_init-demixed",
            "q": 2,
            "c": 6,
            "dependency": "cyclic",
            "init_density": "demixed",
        },
    ]

    for run in runs:
        os.makedirs(f"slurm/{run['name']}", exist_ok=True)

        for rs in rs_list:
            content = f"""#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/{run['name']}/R-%x.%j.out
#SBATCH --error=slurm/{run['name']}/R-%x.%j.err

bash server_run.sh "configs/{run['name']}/q{run['q']}_c{run['c']}_rs{rs}.txt"
"""
            with open(f"slurm/{run['name']}/q{run['q']}_c{run['c']}_rs{rs}.slurm", "w") as f:
                f.write(content)


def generate_rs_sweep_logistic():
    rs_list = [0, 1, 2, 4, 10, 20, 30, 40, 47, 48, 60, 61, 70]

    run_name = "alpha7_p0-0.6_rs-sweep_symmetric_init-demixed_logistic"

    os.makedirs(f"slurm/{run_name}", exist_ok=True)

    for rs in rs_list:
        content = f"""#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=48:00:00

#SBATCH --output=slurm/{run_name}/R-%x.%j.out
#SBATCH --error=slurm/{run_name}/R-%x.%j.err

bash server_run.sh "configs/{run_name}/alpha7_p0-0.6_rs{rs}.txt"
"""
        with open(f"slurm/{run_name}/alpha7_p0-0.6_rs{rs}.slurm", "w") as f:
            f.write(content)


if __name__ == "__main__":
    generate_rs_sweep_power()
    generate_rs_sweep_logistic()

