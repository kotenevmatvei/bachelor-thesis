from matplotlib import pyplot as plt
from matplotlib.figure import figaspect
import numpy as np
import pandas as pd
import seaborn as sns

# read the data
with open("runs/test/data/counts_power_symmetric_reflecting_init-uniform_q3_c5_dt1e-05_nt500_nr100000_rs0_bins100_ft1.txt") as f:
    lines = f.readlines()

n_realizations = 100000
bin_width = 2/100
data = [np.fromstring(line, sep=" ") for line in lines][:-3]
data = [counts / (n_realizations * bin_width) for counts in data[::3]]
df = pd.DataFrame.from_records(data)
df = df.transpose().iloc[:, 1:]
print(df.info())
print(df.shape)
print(df)
print("x len:", len(data[1]))
print("y len:", len(data))

fig, ax = plt.subplots()
fig.set_figheight(10)
fig.set_figwidth(26)

# im = ax.imshow(data)

sns.heatmap(df)

plt.show()

plt.clf()

sns.lineplot(data[300])

plt.show()

