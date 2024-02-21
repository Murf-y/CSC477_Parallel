import matplotlib.pyplot as plt

array_size_vs_parllel_time_per_core = {
    300000: [
        29,
        17,
        14,
        12,
        11,
        11,
        10,
        11
    ],
    400000: [
        41,
        25,
        18,
        18,
        15,
        13,
        13,
        13
    ],
    500000: [
        50,
        30,
        25,
        20,
        19,
        18,
        16,
        16
    ]
}


for array_size, parllel_time_per_core in array_size_vs_parllel_time_per_core.items():
    plt.plot(parllel_time_per_core, marker='o', linestyle='-',
             label=f'{array_size} elements')

plt.xlabel('Number of Cores')
plt.ylabel('Time (s)')
plt.title('Parallel Time per Core')
plt.grid()
plt.legend()
plt.savefig('parllel_time_per_core.png')
