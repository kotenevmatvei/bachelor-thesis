import numpy as np

def gen_hist_test_cases():
    test_cases = [
        ([1.2, 0, -23, 12, 245], 3),
        ([1.44], 3),
        ([1.22, 333, 145, 143, 142, 144, -22], 1),
        ([1.22, 333, 145, 143, 142, 144, -22], 2),
        ([1.22, 333, 145, 143, 142, 144, -22], 9)
    ]

    for test_case in test_cases:
        counts, bins = np.histogram(test_case[0], bins=test_case[1])
        print(counts)
        print(bins)

def parse_config(config_name: str):
    config = {}
    str_keys = ["type"]
    float_keys = ["delta_t", "start", "lower_bound", "upper_bound", "d", "c"]
    int_keys = ["n_t", "n_realizations", "n_bins", "q", "rs", "frame_timestep"]
    with open(f"configs/{config_name}.txt") as file:
        lines = file.readlines()
    for line in lines:
        key, value = line.split(" ")
        if key in float_keys:
            config[key] = float(value)
        elif key in int_keys:
            config[key] = int(value)
        elif key in str_keys:
            config[key] = value[:-1]
            print(config[key])
        else:
            raise ValueError(f"Unknown key: {key}")

    return config


