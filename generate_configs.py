import os

q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3, 4]
q2_c = [4, 5, 5.1, 5.2, 5.25, 5.3, 5.33, 5.333, 5.34, 5.345, 5.4, 5.5, 5.6, 5.8, 6, 7]

print(f"q3: {len(q3_c)}, q2: {len(q2_c)}")

run_name = "c1_q2-3_sweep_symmetric_init_demixed"

os.makedirs(f"configs/{run_name}", exist_ok=True)

for q_val in [2, 3]:
    for c_val in q3_c:
        c_str = str(c_val)
        content = f"""type s3d_correct
run {run_name}
dependency symmetric
init_density demixed
delta_t 0.0001
start 0
lower_bound -1
upper_bound 1
d 1
n_t 100000000
n_realizations 10000
n_bins 100
c {c_str}
q {q_val}
rs 0
frame_timestep 10000
"""
        with open(f"configs/{run_name}/q{q_val}_c{c_val}.txt", "w") as f:
            f.write(content)
