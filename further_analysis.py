import numpy as np
from scipy.signal import argrelextrema
from matplotlib import pyplot as plt

BIN_WIDTH = 0.02
PERFECT_ORDER = 50
MAX_MID_WIDTH = 100


def peak_width_analytical(c, low_level=0.103):
    h = low_level + 1 / (c * low_level)
    En = h * (1 + 4 * c * low_level**2)
    m = np.sinh((1 / 3) * np.arcsinh(3 * En * np.sqrt(3 * c))) / np.sqrt(3 * c)
    w = (1 - 2 * m) / (h + 2 * low_level - 3 * m)
    return w


def get_last_mean_densities(run_name, n_realizations, n_last_rows=10):
    data_filename = f"runs/{run_name}/data/cnts_{run_name}.txt"

    with open(data_filename, "rb") as f:
        lines = f.readlines()

    last_counts_rows = [
        np.fromstring(line, sep=" ", dtype=np.float64) for line in lines
    ][-3 * n_last_rows :]

    # normalize and drop first element (timestep index)
    last_densities = [
        counts[1:] / (n_realizations * BIN_WIDTH) for counts in last_counts_rows
    ]
    densities = {
        "A_species": np.mean(last_densities[0::3], axis=0),
        "B_species": np.mean(last_densities[1::3], axis=0),
        "C_species": np.mean(last_densities[2::3], axis=0),
    }
    # densities = {
    #     "A_species": last_densities[-3],
    #     "B_species": last_densities[-2],
    #     "C_species": last_densities[-1],
    # }
    # plt.plot(np.linspace(-1, 1, 100), densities["B_species"])
    # plt.show()
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
        raise ValueError(f"Interfaces must be 4, got {len(indices)}")

    high_width = (indices[2] - indices[1]) * BIN_WIDTH
    middle_width = (indices[1] - indices[0] + indices[3] - indices[2]) * BIN_WIDTH
    low_width = (indices[0] + 100 - indices[3]) * BIN_WIDTH

    return high_width, middle_width, low_width, spreads


def main():
    c_vals = [3, 4, 5, 5.2, 5.3, 5.33, 5.5, 5.6, 6, 7]
    orders = [0.0, 0.0]
    middle_widths = [1.0, 1.0]
    spreads_rows = []
    mid_densities = []
    peak_widths = [0, 0]
    for c_val in c_vals:
        run_name = (
            f"pow_sym_ref_init-dem_q2_c{c_val}_dt1e-05_"
            f"nr100000_rs0_bins100_cnts-step1000"
        )
        print(f"Calculating run {run_name}")
        densities = get_last_mean_densities(run_name, 100000, 70)
        mid_density = densities["B_species"]
        mid_densities.append(mid_density)

        # for species, density in densities.items():
        #     peak_position = assign_peak_position(density)
        #     if peak_position == "center":
        #         print(f"Center peak assumed by {species}")
        #         mid_density = density

        if c_val not in [3, 4]:
            order = calculate_order_parameter(mid_density)
            peak_width = get_level_widths(mid_density)[0]
            spreads = get_level_widths(mid_density)[3]
            middle_width = get_level_widths(mid_density)[1]
            middle_widths.append(middle_width)
            orders.append(order)
            peak_widths.append(peak_width)
            spreads_rows.append(spreads)

    plt.plot(np.linspace(-1, 1, 94), spreads_rows[-1])
    plt.title("Spreads")
    plt.xlabel("Coupling strength $c$")
    plt.ylabel("$spread$")
    plt.grid(True)
    plt.savefig("figures/spreads.png", dpi=300)
    plt.clf()

    plt.plot(c_vals, peak_widths, "-o")
    w_analytical = [peak_width_analytical(c) for c in c_vals]
    plt.plot(c_vals, w_analytical, "-o")
    plt.title("Peak width vs coupling strength $c$")
    plt.xlabel("Coupling strength $c$")
    plt.ylabel("Peak width W")
    plt.grid(True)
    plt.savefig("figures/peak_widths.png", dpi=300)
    plt.clf()

    plt.plot(c_vals, middle_widths, "-o")
    plt.title("Normed total width of middle density $p_{mid}$")
    plt.xlabel("Coupling strength $c$")
    plt.ylabel("$p_\\text{mid} * 0.01$")
    plt.grid(True)
    plt.savefig("figures/middle_widths.png", dpi=300)
    plt.clf()

    # add the perfect order line!
    plt.plot(c_vals, orders, "-o")
    plt.title("Normed order parameter of the middle density $p_{mid}$")
    plt.xlabel("Coupling strength $c$")
    plt.ylabel("$O(p_{mid}) * 0.02$")
    plt.grid(True)
    plt.savefig("figures/orders.png", dpi=300)
    plt.clf()

    for c_val, density in zip(c_vals, mid_densities):
        plt.plot(np.linspace(-1, 1, 100), density, label=f"$c$={c_val}")
    plt.title("Final middle density vs coupling strength $c$")
    plt.xlabel("Coupling strength $c$")
    plt.ylabel("$p$")
    plt.grid(True)
    plt.legend()
    plt.savefig("figures/mid_densities.png", dpi=300)
    plt.clf()


if __name__ == "__main__":
    main()
