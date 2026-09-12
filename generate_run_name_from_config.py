import argparse
from helpers import parse_config
from plot_tripple_diffusion import build_config_name

def main():
    parser = argparse.ArgumentParser(description="Get config name")
    parser.add_argument("config_path", help="The name of the config file")
    args = parser.parse_args()
    config_path = args.config_path
    config = parse_config(config_path)

    type_ = config["type"]
    run = config["run"]
    delta_t = config["delta_t"]
    start = config["start"]
    upper_bound = config["upper_bound"]
    lower_bound = config["lower_bound"]
    d = config["d"]
    n_t = config["n_t"]
    n_realizations = config["n_realizations"]
    n_bins = config["n_bins"]
    c = config["c"]
    q = config["q"]
    rs = config["rs"]
    p_0 = config["p_0"]
    alpha = config["alpha"]
    cnts_timestep = config["cnts_timestep"]
    dependency = config["dependency"]
    boundary = config["boundary"]
    init_density = config["init_density"]
    config_name = build_config_name(
        type_,
        dependency,
        run,
        boundary,
        init_density,
        delta_t,
        n_t,
        n_realizations,
        n_bins,
        upper_bound,
        lower_bound,
        c,
        q,
        p_0,
        alpha,
        rs,
        cnts_timestep,
    )

    print(config_name)

if __name__ == "__main__":
    main()
