import os

q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3, 4]
q2_c = [4, 5, 5.1, 5.2, 5.25, 5.3, 5.33, 5.333, 5.34, 5.345, 5.4, 5.5, 5.6, 5.8, 6, 7]

p_0_list = [0, 0.2, 0.4, 0.5, 0.6, 0.8, 1]

print(f"q3: {len(q3_c)}, q2: {len(q2_c)}")

run_name = "q2-3_c_sweep_symmetric_init-uniform"

power_run_names = [
    # "q2-3_c_sweep_symmetric_init-uniform",  # already run
    "q2-3_c_sweep_symmetric_init-demixed",
    "q2-3_c_sweep_cyclic_init-uniform",
]

for run_name in power_run_names:
    os.makedirs(f"configs/{run_name}", exist_ok=True)

    for q_val in [2, 3]:
        c_list = q2_c if q_val == 2 else q3_c
        for c_val in c_list:
            content = f"""type power
run {run_name}
dependency symmetric
boundary reflecting
init_density uniform
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
frame_timestep 10000
alpha 8
p_0 0.35
"""
            with open(f"configs/{run_name}/q{q_val}_c{c_val}.txt", "w") as f:
                f.write(content)


run_name = "alpha7-8_p0_sweep_symmetric_init-demixed"

os.makedirs(f"configs/{run_name}", exist_ok=True)

for alpha in [7, 8]:
    for p_0 in p_0_list:
        content = f"""type logistic
run {run_name}
dependency symmetric
boundary reflecting
init_density demixed
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
frame_timestep 10000
alpha {alpha}
p_0 {p_0}
"""
        with open(f"configs/{run_name}/alpha{alpha}_p0{p_0}.txt", "w") as f:
            f.write(content)
