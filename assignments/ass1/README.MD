# Benchmarking Parallel Compression

---

This project aims to benchmark the times it takes to compress `N` files with different strategies.

## Strategies

Parallelize the compression of N files using multiple methods:

1. Creating N parallel tasks at same time to proccess the N files

2. Creating NB_CORES tasks each time to proccess NB_CORES files, when all the files are proccessed, create another NB_CORES taks to proccess the next NB_CORES files, and so on until all the files are proccessed.

3. Creating NB_CORES tasks at all, and assign multiple files compression for each task (as equal as possible)

## How to run

### Scripts

Creates `<number of duplicates>` copies of `<filename>` with the same content and places them in `dups` folder.

```bash
./scripts/create_dups.sh <filename> <number of duplicates>
```

Clean the dups folder after each run from the zipped files created.

```bash
./scripts/clean_dups_from_zipped.sh dups
```

### Benchmark

To run the benchmark, you need to compile the `main.c` file and run it with the name of the folder where the files are located.

```bash
gcc main.c -o main ; ./main dups
```

1. Make sure to uncomment this part if u want to try sequential compression:

```c
printf("Starting sequential compression %d times\n", numberOfRuns);
double sequentialTime = sequential(filesList, numberOfRuns);
printf("Avg. Sequential time after %d runs: %f\n", numberOfRuns, sequentialTime);
clean_up(argv[1]);
```

2. Make sure to uncomment this part if u want to specify a specific number of cores to use and split tasks between them:

```c
for (int i = 2; i <= getNumCPUs(); i++)
{
    printf("Starting parallel compression with  %d cores %d times\n", i, numberOfRuns);
    double parallelTime = parallelWithCores(filesList, numberOfRuns, i, numberOfFiles);
    printf("Avg. Parallel time after %d runs: %f\n", numberOfRuns, parallelTime);
    clean_up(argv[1]);
}
```

3. Make sure to uncomment this part and pick a strategy (1, 2, or 3) if u want to try parallel compression:

```c
int strategy = 1;
printf("Starting strategy %d parallel compression %d times\n", strategy, numberOfRuns);
double parallelTime = parallelRunWithStrategy(filesList, numberOfRuns, strategy);
printf("Avg. Parallel time after %d runs: %f\n", numberOfRuns, parallelTime);
clean_up(argv[1]);    
```

## Results

In `logs` folder check the plots for the results of the benchmark, 3D and 2D plot.

Additionally, you can run the python file:

```bash
python3 plot_graph.py
```

Or view the excel file directly: `runtime.xslx`
