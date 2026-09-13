import numpy as np


def gen_hist_test_cases():
    test_cases = [
        ([1.2, 0, -23, 12, 245], 3),
        ([1.44], 3),
        ([1.22, 333, 145, 143, 142, 144, -22], 1),
        ([1.22, 333, 145, 143, 142, 144, -22], 2),
        ([1.22, 333, 145, 143, 142, 144, -22], 9),
    ]

    for test_case in test_cases:
        cnts, bins = np.histogram(test_case[0], bins=test_case[1])
        print(cnts)
        print(bins)

# maybe unnecessary, leave for now
def parse_config_essentials_from_name(name: str):
    str_keys = ["type", "run", "dependency", "boundary", "init_density"]
    float_keys = [
        "delta_t",
        "start",
        "lower_bound",
        "upper_bound",
        "d",
        "c",
        "p_0",
        "alpha",
    ]
    int_keys = [
        "n_t",
        "n_realizations",
        "n_bins",
        "q",
        "rs",
        "cnts_timestep",
        "crds_snapshot",
    ]
    tokens = name.split("_")
    config = {}
    config["type"] = "pow" if "pow" in name else "log"
    config["dependency"] = "sym" if "sym" in name else "cy"
    config["boundary"] = "ref" if "ref" in name else "per"
    config["init_density"] = "dem" if "dem" in name else "un"
    config["dt"] = float([token for token in tokens if token.startswith("dt")][0][2:])
    config["n_realizations"] = int([token for token in tokens if token.startswith("nr")][0][2:])
    config["nr"] = int([token for token in tokens if token.startswith("nr")][0][2:])
    config["n_bins"] = int([token for token in tokens if token.startswith("bins")][0][4:])

    print(config)
    print(type(config["dt"]))


    




def parse_config(config_name: str):
    config = {}
    str_keys = ["type", "run", "dependency", "boundary", "init_density"]
    float_keys = [
        "delta_t",
        "start",
        "lower_bound",
        "upper_bound",
        "d",
        "c",
        "p_0",
        "alpha",
    ]
    int_keys = [
        "n_t",
        "n_realizations",
        "n_bins",
        "q",
        "rs",
        "cnts_timestep",
        "crds_snapshot",
    ]
    with open(config_name) as file:
        lines = file.readlines()
    for line in lines:
        if line == "\n":  # from here starts the comments section
            break
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

parse_config_essentials_from_name("pow_sym_ref_init-un_q2_c9_dt1e-05_nr100000_rs0_bins100_cnts-step1000.txt")
