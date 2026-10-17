import numpy as np
from matplotlib import pyplot as plt

BIN_WIDTH = 0.02


def read_counts(run_name: str) -> tuple[list[np.ndarray], ...]:
    """The counts and timestep index (0th element) for each species are stored on every
    third position, so the structure is:
    A_i0 A_bin0 ... A_bin99
    B_i0 B_bin0 ... B_bin99
    C_i0 C_bin0 ... C_bin99
    A_i1 A_bin0 ... A_bin99
    ...
    """
    data_filename = f"runs/{run_name}/data/cnts_{run_name}.txt"

    with open(data_filename, "rb") as f:
        lines = f.readlines()

    A_cnts_list = [np.fromstring(line, sep=" ", dtype=np.float64) for line in lines[0::3]][1:]
    B_cnts_list = [np.fromstring(line, sep=" ", dtype=np.float64) for line in lines[1::3]][1:]
    C_cnts_list = [np.fromstring(line, sep=" ", dtype=np.float64) for line in lines[2::3]][1:]

    return A_cnts_list, B_cnts_list, C_cnts_list


def normalize_counts_drop_index(
    counts_rows: list[np.ndarray], n_realizations: int
) -> list[np.ndarray]:
    """Drop first element of every array (timestep index) and normalize to get density"""
    density = [counts[1:] / (n_realizations * BIN_WIDTH) for counts in counts_rows]
    return density


def calculate_order_parameter(density: np.ndarray) -> float:
    """Order parameter is defined as the integral over the deviation from homogeneous
    state sqared
    """
    order = np.sum((0.5 - density) ** 2)
    return order


def plot_order_parameter_for_evolution(density_rows: list[np.ndarray]):
    orders = []
    for density in density_rows:
        order = calculate_order_parameter(density)
        orders.append(order)

    fig, axes = plt.subplots()
    axes.plot(range(len(orders)), orders)
    plt.show()

def main():
    run_name = [
        "pow_sym_ref_init-un_q2_c5.6_dt1e-05_nr100000_rs0_bins100_cnts-step1000",
        "pow_sym_ref_init-un_q2_c6_dt0.0001_nr10000_rs0_bins100_cnts-step1000",
        "pow_sym_ref_init-dem_q2_c6_dt1e-05_nr100000_rs0_bins100_cnts-step1000"
    ]

    nr = 100000
    _, counts_rows, _ = read_counts(run_name[0])
    density_rows = normalize_counts_drop_index(counts_rows, nr)
    plot_order_parameter_for_evolution(density_rows)

if __name__ == "__main__":
    order = calculate_order_parameter(np.array(([1.5, 0, 1])))
    print(order)
    main()
