q3_c = [1, 1.5, 1.7, 1.9, 1.95, 1.97, 1.99, 2, 2.01, 2.03, 2.05, 2.1, 2.3, 2.5, 3, 4]
q2_c = [4, 5, 5.1, 5.2, 5.25, 5.3, 5.33, 5.333, 5.34, 5.345, 5.4, 5.5, 5.6, 5.8, 6, 7]

print(f"q3: {len(q3_c)}, q2: {len(q2_c)}")

for c in q3_c:
    c_str = str(c)
    content = f"type s3d_correct\ndelta_t 0.0001\nstart 0\nlower_bound -1\nupper_bound 1\nd 1\nn_t 100000000\nn_realizations 10000\nn_bins 100\nc {c_str}\nq 3\nrs 0\nframe_timestep 10000"
    with open(f"configs/q3_c{c}.txt", "w") as f:
        f.write(content)

for c in q2_c:
    c_str = str(c)
    content = f"type s3d_correct\ndelta_t 0.0001\nstart 0\nlower_bound -1\nupper_bound 1\nd 1\nn_t 100000000\nn_realizations 10000\nn_bins 100\nc {c_str}\nq 2\nrs 0\nframe_timestep 10000"
    with open(f"configs/q2_c{c}.txt", "w") as f:
        f.write(content)
