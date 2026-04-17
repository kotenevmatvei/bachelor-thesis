import numpy as np

test_cases = [
    ([1.2, 0, -23, 12, 245], 3),
    ([1.44], 3),
    ([1.22, 333, 145, 143, 142, 144, -22], 1),
    ([1.22, 333, 145, 143, 142, 144, -22], 2),
    ([1.22, 333, 145, 143, 142, 144, -22], 9)
]

for test_case in test_cases:
    counts, bins = np.histogram(test_case[0], bins=test_case[1])
    print(counts)
    print(bins)


