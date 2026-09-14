import os

q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3, 4]
q2_c = [4, 5, 5.1, 5.2, 5.25, 5.3, 5.33, 5.333, 5.34, 5.345, 5.4, 5.5, 5.6, 5.8, 6, 7]

p_0_list = [0, 0.2, 0.4, 0.5, 0.6, 0.8, 1]

print(f"q3: {len(q3_c)}, q2: {len(q2_c)}")


def generate_c_sweep():
    pow_run_names = [
        "q2-3_c_sweep_sym_init-un",  # already run
        "q2-3_c_sweep_sym_init-dem",  # already run
        "q2-3_c_sweep_cy_init-un",  # already run
    ]

    for run_name in pow_run_names:
        os.makedirs(f"configs/{run_name}", exist_ok=True)

        for q_val in [2, 3]:
            c_list = q2_c if q_val == 2 else q3_c
            for c_val in c_list:
                content = f"""type pow
run {run_name}
dependency sym
boundary ref
init_density un
delta_t 0.0001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 100000000
n_realizations 10000
n_bins 100
c {c_val}
q {q_val}
rs 0
cnts_timestep 10000
alpha 8
p_0 0.35
"""
                with open(f"configs/{run_name}/q{q_val}_c{c_val}.txt", "w") as f:
                    f.write(content)


def generate_p0_sweep():
    run_name = "alpha7-8_p0_sweep_sym_init-dem"

    os.makedirs(f"configs/{run_name}", exist_ok=True)

    for alpha in [7, 8]:
        for p_0 in p_0_list:
            content = f"""type log
run {run_name}
dependency sym
boundary ref
init_density dem
delta_t 0.0001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 100000000
n_realizations 10000
n_bins 100
c 3
q 3
rs 0
cnts_timestep 10000
alpha {alpha}
p_0 {p_0}
"""
            with open(f"configs/{run_name}/alpha{alpha}_p0{p_0}.txt", "w") as f:
                f.write(content)


def generate_rs_sweep_pow():
    rs_list = [0, 1, 2, 4, 10, 20, 30, 40, 47, 48, 60, 61, 70]
    runs = [
        {
            "name": "q3_c3_rs-sweep_sym_init-un",
            "q": 3,
            "c": 3,
            "dependency": "sym",
            "init_density": "un",
        },
        {
            "name": "q3_c3_rs-sweep_sym_init-un",
            "q": 3,
            "c": 3,
            "dependency": "sym",
            "init_density": "dem",
        },
        {
            "name": "q2_c6_rs-sweep_sym_init-un",
            "q": 2,
            "c": 6,
            "dependency": "sym",
            "init_density": "un",
        },
        {
            "name": "q2_c6_rs-sweep_sym_init-dem",
            "q": 2,
            "c": 6,
            "dependency": "sym",
            "init_density": "dem",
        },
        {
            "name": "q3_c3_rs-sweep_cy_init-un",
            "q": 3,
            "c": 3,
            "dependency": "cy",
            "init_density": "un",
        },
        {
            "name": "q3_c3_rs-sweep_cy_init-un",
            "q": 3,
            "c": 3,
            "dependency": "cy",
            "init_density": "dem",
        },
        {
            "name": "q2_c6_rs-sweep_cy_init-un",
            "q": 2,
            "c": 6,
            "dependency": "cy",
            "init_density": "un",
        },
        {
            "name": "q2_c6_rs-sweep_cy_init-dem",
            "q": 2,
            "c": 6,
            "dependency": "cy",
            "init_density": "dem",
        },
    ]

    for run in runs:
        os.makedirs(f"configs/{run['name']}", exist_ok=True)

        for rs in rs_list:
            content = f"""type pow
run {run["name"]}
dependency {run["dependency"]}
boundary ref
init_density {run["init_density"]}
delta_t 0.0001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 100000000
n_realizations 10000
n_bins 100
c {run["c"]}
q {run["q"]}
rs {rs}
cnts_timestep 10000
alpha 8
p_0 0.35
"""
            with open(
                f"configs/{run['name']}/q{run['q']}_c{run['c']}_rs{rs}.txt", "w"
            ) as f:
                f.write(content)


def generate_rs_sweep_log():
    run_name = "alpha7_p0-0.6_rs-sweep_sym_init-dem_log"
    rs_list = [0, 1, 2, 4, 10, 20, 30, 40, 47, 48, 60, 61, 70]

    os.makedirs(f"configs/{run_name}", exist_ok=True)

    for rs in rs_list:
        content = f"""type log
run {run_name}
dependency sym
boundary ref
init_density dem
delta_t 0.0001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 100000000
n_realizations 10000
n_bins 100
c 3
q 3
rs {rs}
cnts_timestep 10000
alpha 7
p_0 0.6
"""
        with open(f"configs/{run_name}/alpha7_p0-0.6_rs{rs}.txt", "w") as f:
            f.write(content)


def generate_c_sweep_for_bistability():
    c_list = [3, 4, 5, 5.2, 5.3, 5.33, 5.5, 5.6, 6, 7, 9]
    # init-demixed
    for init in ["dem", "un"]:
        for c_val in c_list:
            run_name = f"pow_sym_ref_init-{init}_q2_c{c_val}_dt1e-05_nr100000_rs0_bins100_cnts-step1000"

            content = f"""run {run_name}
type pow
dependency sym
boundary ref
init_density {init}
delta_t 0.00001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 10000000
n_realizations 100000
n_bins 100
c {c_val}
q 2
rs 0
cnts_timestep 1000
crds_snapshot 10000
alpha 3
p_0 0.2


comments:

type: log or pow
dependency: sym or cy
boundary: ref or per
init_density: un or dem
"""
            with open(f"configs/{run_name}.txt", "w") as f:
                f.write(content)
            print(run_name)

def generate_c_sweep_for_bistability_cyclic_periodic():
    c_list = [3, 4, 5, 5.2, 5.3, 5.33, 5.5, 5.6, 6, 7, 9]
    # init-demixed
    for init in ["dem", "un"]:
        for c_val in c_list:
            run_name = f"pow_cy_per_init-{init}_q2_c{c_val}_dt1e-05_nr100000_rs0_bins100_cnts-step1000"

            content = f"""run {run_name}
type pow
dependency cy
boundary per
init_density {init}
delta_t 0.00001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 10000000
n_realizations 100000
n_bins 100
c {c_val}
q 2
rs 0
cnts_timestep 1000
crds_snapshot 10000
alpha 3
p_0 0.2


comments:

type: log or pow
dependency: sym or cy
boundary: ref or per
init_density: un or dem
"""
            with open(f"configs/{run_name}.txt", "w") as f:
                f.write(content)
            print(run_name)


def generate_q2_c7():
    # init-demixed
    for init in ["dem", "un"]:
        run_name = f"pow_sym_ref_init-{init}_q2_c7_dt1e-05_nr1000000_rs0_bins100_cnts-step1000"

        content = f"""run {run_name}
type pow
dependency sym
boundary ref
init_density {init}
delta_t 0.00001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 2000000
n_realizations 1000000
n_bins 100
c 7
q 2
rs 0
cnts_timestep 1000
crds_snapshot 10000
alpha 3
p_0 0.2


comments:

type: log or pow
dependency: sym or cy
boundary: ref or per
init_density: un or dem
"""
        with open(f"configs/{run_name}.txt", "w") as f:
            f.write(content)
        print(run_name)


if __name__ == "__main__":
    generate_q2_c7()
