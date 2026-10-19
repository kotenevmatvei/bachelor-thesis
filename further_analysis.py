import numpy as np
from scipy.signal import argrelextrema
from matplotlib import pyplot as plt

BIN_WIDTH = 0.02


def get_last_mean_density_from_counts(run_name, n_realizations, n_last_rows=10):
    data_filename = f"runs/{run_name}/data/cnts_{run_name}.txt"

    with open(data_filename, "rb") as f:
        lines = f.readlines()

    last_counts_rows = [np.fromstring(line, sep=" ", dtype=np.float64) for line in lines[-1 * n_last_rows :: 3]]

    # normalize and drop first element (timestep index)
    last_density_rows = [counts[1:] / (n_realizations * BIN_WIDTH) for counts in last_counts_rows]

    last_mean_density = np.mean(last_density_rows, axis=0)

    return last_mean_density


def calculate_order_parameter(density):
    order_parameter = np.sum((0.5 - density) ** 2)
    return order_parameter


def get_level_widths(density, n_interfaces):
    # calculate rolling spread
    spreads = []
    for ind in range(3, len(density) - 3):
        slice = density[ind - 3 : ind + 3]
        spread = max(slice) - min(slice)
        spreads.append(spread)

    # indices = np.where(np.isin(spreads, maxima))
    # indices = [spreads.index(maximum) for maximum in maxima]
    indices = argrelextrema(np.array(spreads), np.greater, order=5)[0]
    high_width = indices[2] - indices[1]
    middle_width = indices[1] - indices[0] + indices[3] - indices[2]
    low_width = indices[0] + 3 + 100 - indices[3]

    return high_width, middle_width, low_width


def main():
    c_vals = [3, 4, 5, 5.2, 5.3, 5.33, 5.5, 5.6, 6, 7, 9]
    run_names = [f"pow_sym_ref_init-dem_q2_c{c_val}_dt1e-05_nr100000_rs0_bins100_cnts-step1000" for c_val in c_vals]
    densities = [get_last_mean_density_from_counts(run_name, 100000, 10) for run_name in run_names]
    level_widths = get_level_widths(densities[5], 4)
    print(level_widths)

    orders = [calculate_order_parameter(density) for density in densities]
    # plt.plot(c_vals, orders, "-o")
    # add the perfect order line!
    plt.show()


if __name__ == "__main__":
    main()
