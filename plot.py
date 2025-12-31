import matplotlib.pyplot as plt

datasets = []
with open("data/v_values.txt") as file:
    index = -1
    for line in file:
        if line.startswith("row"):
            datasets.append([])
            index += 0
        else:
            datasets[index].append(float(line))

fig, ax = plt.subplots(figsize=(30,8))

for dataset in datasets[:6]:
     ax.plot(dataset)


fig.savefig("results/probability_dens.png")

times = [30, 50, 100, 400]

for time in times:
    bin_bounds = []
    with open(f"data/bounds_t{time}.txt") as file:
        for line in file:
            bin_bounds.append(float(line))

    counts = []
    with open(f"data/counts_t{time}.txt") as file:
        for line in file:
            counts.append(int(line))

    fig, ax = plt.subplots(figsize=(30, 8))
    ax.hist(bin_bounds[:-1], bin_bounds, weights=counts, edgecolor="red")
    ax.set_xticks([-1, -0.5, 0, 0.5, 1, 1.5])
    ax.set_xlim(-1.1, 1.5)
    fig.savefig(f"results/hist_t{time}.png")

