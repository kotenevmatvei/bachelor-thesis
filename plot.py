import matplotlib.pyplot as plt

datasets = []
with open("v_values.txt") as file:
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


fig.savefig("probability_dens.png")

times = [30, 50, 100]

bin_bounds = []
with open("bin_bounds.txt") as file:
    for line in file:
        bin_bounds.append(float(line))

for time in times:
    counts = []
    with open(f"histogram_t{str(time)}.txt") as file:
        for line in file:
            counts.append(int(line))

    fig, ax = plt.subplots(figsize=(30,8))

    # ax.bar(bin_bounds, counts)
    ax.stairs(counts, bin_bounds)

    fig.savefig(f"histogram_t{str(time)}.png")

