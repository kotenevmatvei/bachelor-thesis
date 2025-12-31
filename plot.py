import matplotlib.pyplot as plt

datasets = []
# with open("v_values.txt") as file:
#     index = -1
#     for line in file:
#         if line.startswith("row"):
#             datasets.append([])
#             index += 0
#         else:
#             datasets[index].append(float(line))
# 
# fig, ax = plt.subplots(figsize=(30,8))
# 
# for dataset in datasets[:6]:
#      ax.plot(dataset)
# 
# 
# fig.savefig("probability_dens.png")
# 
# times = [30, 50, 100]

bin_bounds = []
with open("bin_bounds.txt") as file:
    for line in file:
        bin_bounds.append(float(line))

counts = []
with open("histogram_t30.txt") as file:
    for line in file:
        counts.append(int(line))

fig, ax = plt.subplots(figsize=(30, 8))
ax.hist(bin_bounds[:-1], bin_bounds, weights=counts, edgecolor="red")
ax.set_xticks([-1, -0.5, 0, 0.5, 1, 1.5])
ax.set_xlim(-1.1, 1.5)
fig.savefig("histogram_t30.png")

    ## for time in times:
    ##     counts = []
    ##     with open(f"histogram_t{str(time)}.txt") as file:
    ##         for line in file:
    ##             counts.append(int(line))
    ## 
    ##     fig, ax = plt.subplots(figsize=(30,8))
    ## 
    ##     ax.bar(bin_bounds[1:], counts)
    ##     # ax.stairs(counts, bin_bounds)
    ## 
    ##     fig.savefig(f"histogram_t{str(time)}.png")

