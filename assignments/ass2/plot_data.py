import matplotlib.pyplot as plt
import numpy

# For each array size, for each core, set the time taken for that core
array_size_vs_parllel_time_per_core = {
    100000:
        [15,
         8,
         8,
         8,
         7,
         8,
         7,
         7,
         8,
         6,
         8,
         7,
         7,
         7,
         8,
         8,],
    500000:
        [83,
         42,
         30,
         26,
         23,
         26,
         25,
         25,
         25,
         25,
         24,
         24,
         25,
         24,
         26,
         28],
    1000000:
        [191,
         91,
         63,
         59,
         54,
         46,
         46,
         55,
         53,
         45,
         46,
         45,
         43,
         46,
         46,
         46,],
    5000000:
    [1063,
        472,
        343,
        319,
        288,
        279,
        256,
        270,
        276,
        273,
        270,
        280,
        255,
        249,
        253,
        255],
    10000000:
        [1964,
         960,
         746,
         634,
         563,
         603,
         513,
         561,
         602,
         563,
         561,
         588,
         570,
         543,
         537,
         560]

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
plt.clf()

array_sizes = list(array_size_vs_parllel_time_per_core.keys())

# For each array size, the time taken by sequential code
array_size_vs_sequential = [
    15,
    77,
    168,
    1022,
    1773,
]

# For each core, for each array size, set the time taken by that core
core_vs_array_size_parallel = {
    core: [] for core in range(1, 16 + 1)
}

for array_size, parllel_time_per_core in array_size_vs_parllel_time_per_core.items():
    for core, time in enumerate(parllel_time_per_core, start=1):
        core_vs_array_size_parallel[core].append(time)

plt.figure(figsize=(10, 10))
for core, times in core_vs_array_size_parallel.items():
    plt.plot(times, marker='o', linestyle='-',
             label=f'{core} cores')


plt.plot(array_size_vs_sequential, marker='o', linestyle='-',
         label=f'Sequential')

plt.xticks(numpy.arange(len(array_sizes)), array_sizes)


plt.xlabel('Array Size')
plt.ylabel('Time (s)')
plt.title('Parallel Time vs Array Size')
plt.grid()
plt.legend()
plt.savefig('parallel_time_vs_array_size.png')
