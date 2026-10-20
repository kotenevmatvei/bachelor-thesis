import numpy as np
from scipy.signal import argrelextrema
from matplotlib import pyplot as plt

BIN_WIDTH = 0.02
PERFECT_ORDER = 50
MAX_MID_WIDTH = 100


def get_last_mean_densities(run_name, n_realizations, n_last_rows=10):
    data_filename = f"runs/{run_name}/data/cnts_{run_name}.txt"

    with open(data_filename, "rb") as f:
        lines = f.readlines()

    last_counts_rows = [
        np.fromstring(line, sep=" ", dtype=np.float64) for line in lines
    ][-3 * n_last_rows :]

    # normalize and drop first element (timestep index)
    last_densities = [
        counts[1:] / (n_realizations * BIN_WIDTH)
        for counts in last_counts_rows
    ]
    densities = {
        "A_species": np.mean(last_densities[0::3], axis=0),
        "B_species": np.mean(last_densities[1::3], axis=0),
        "C_species": np.mean(last_densities[2::3], axis=0),
    }
    return densities


def assign_peak_position(density):
    maximum = max(density)
    max_index = list(density).index(maximum)
    if max_index < 25:
        return "left"
    elif 25 < max_index < 75:
        return "center"
    else:
        return "right"


def calculate_order_parameter(density):
    order_parameter = np.sum((0.5 - density) ** 2) / PERFECT_ORDER
    return order_parameter


def get_spreads(density):
    # calculate rolling spread
    spreads = []
    for ind in range(3, len(density) - 3):
        slice = density[ind - 3 : ind + 3]
        spread = max(slice) - min(slice)
        spreads.append(spread)
    return spreads


def get_level_widths(density):
    spreads = get_spreads(density)
    # indices = np.where(np.isin(spreads, maxima))
    # indices = [spreads.index(maximum) for maximum in maxima]
    indices = argrelextrema(np.array(spreads), np.greater, order=6)[0]
    # remoce fake interfaces with too little density jumps
    for ind, index in enumerate(indices):
        slice = spreads[index - 4 : index + 4]
        if max(slice) - min(slice) < 0.2:
            indices = np.delete(indices, ind)
    if len(indices) != 4:
        raise ValueError("Interfaces must be 4")

    high_width = (indices[2] - indices[1]) / MAX_MID_WIDTH
    middle_width = (indices[1] - indices[0] + indices[3] - indices[2]) / MAX_MID_WIDTH
    low_width = (indices[0] + 100 - indices[3]) / MAX_MID_WIDTH

    return high_width, middle_width, low_width, spreads


def main():
    c_vals = [5, 5.2, 5.3, 5.33, 5.5, 5.6, 6, 7]
    run_names = [
        f"pow_sym_ref_init-dem_q2_c{c_val}_dt1e-05_"
        f"nr100000_rs0_bins100_cnts-step1000"
        for c_val in c_vals
    ]
    orders = []
    middle_widths = []
    spreads_rows = []
    mid_densities = []
    for run_name in run_names:
        print(f"Calculating run {run_name}")
        densities = get_last_mean_densities(run_name, 100000)
        mid_density = []
        for species, density in densities.items():
            peak_position = assign_peak_position(density)
            if peak_position == "center":
                print(f"Center peak assumed by {species}")
                mid_density = density

        order = calculate_order_parameter(mid_density)
        middle_width = get_level_widths(mid_density)[1]
        spreads = get_level_widths(mid_density)[3]
        orders.append(order)
        middle_widths.append(middle_width)
        spreads_rows.append(spreads)
        mid_densities.append(mid_density)

    
    plt.plot(c_vals, middle_widths, "-o")
    plt.savefig("figures/middle_widths.png")
    plt.clf()

    # add the perfect order line!
    plt.plot(c_vals, orders, "-o")
    plt.savefig("figures/orders.png")
    plt.clf()

    for density in mid_densities:
        plt.plot(density)
    plt.savefig("figures/mid_densities.png")
    plt.clf()


if __name__ == "__main__":
    main()
