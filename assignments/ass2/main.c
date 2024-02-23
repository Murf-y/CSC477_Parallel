#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdbool.h>

#define RANDOM_SEED 100

#define ARR_LENGTH 10000000
#define UPPER_NUMBER_LIMIT 1000
#define LOWER_NUMBER_LIMIT 1
#define RECURSIVE_LIMIT 100 // At this limit switch to insertion sort

#define NUM_THREADS 16
#define VERBOSE 0

int CHUNK_SIZE_PER_THREAD = ARR_LENGTH / NUM_THREADS;
int OFFSET = ARR_LENGTH % NUM_THREADS;
int *arr;
int *arr_copy;

long long timeInMilliseconds(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

void merge(int left, int mid, int right)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int left_length = mid - left + 1;
    int right_length = right - mid;
    int *left_array = (int *)malloc(left_length * sizeof(int));
    int *right_array = (int *)malloc(right_length * sizeof(int));

    for (int i = 0; i < left_length; i++)
    {
        left_array[i] = arr[left + i];
    }

    for (int j = 0; j < right_length; j++)
    {
        right_array[j] = arr[mid + 1 + j];
    }

    i = 0;
    j = 0;
    while (i < left_length && j < right_length)
    {
        if (left_array[i] <= right_array[j])
        {
            arr[left + k] = left_array[i];
            i++;
        }
        else
        {
            arr[left + k] = right_array[j];
            j++;
        }
        k++;
    }

    while (i < left_length)
    {
        arr[left + k] = left_array[i];
        k++;
        i++;
    }

    while (j < right_length)
    {
        arr[left + k] = right_array[j];
        k++;
        j++;
    }

    free(left_array);
    free(right_array);
}

void mergeSort(int left, int right)
{
    // if size is <= RECURSIVE_LIMIT then use insertion sor because it is faster for smaller arrays
    if (right - left <= RECURSIVE_LIMIT)
    {
        for (int i = left + 1; i <= right; i++)
        {
            int key = arr[i];
            int j = i - 1;
            while (j >= left && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
        return;
    }
    else
    {
        int middle = left + (right - left) / 2;
        mergeSort(left, middle);
        mergeSort(middle + 1, right);
        merge(left, middle, right);
    }
}

void *mergeSortThreaded(void *arg)
{
    int rank = (long)arg;
    int left = rank * CHUNK_SIZE_PER_THREAD;
    int right = (rank + 1) * CHUNK_SIZE_PER_THREAD - 1;

    if (rank == NUM_THREADS - 1)
    {
        right += OFFSET;
    }

    int mid = left + (right - left) / 2;

    if (left < right)
    {
        mergeSort(left, mid);
        mergeSort(mid + 1, right);
        merge(left, mid, right);
    }
    return NULL;
}

void mergeChunks(int number, int aggregation)
{
    for (int i = 0; i < number; i = i + 2)
    {
        int left = i * (CHUNK_SIZE_PER_THREAD * aggregation);
        int right = ((i + 2) * CHUNK_SIZE_PER_THREAD * aggregation) - 1;
        int middle = left + (CHUNK_SIZE_PER_THREAD * aggregation) - 1;
        if (right >= ARR_LENGTH)
        {
            right = ARR_LENGTH - 1;
        }
        merge(left, middle, right);
    }
    if (number / 2 >= 1)
    {
        mergeChunks(number / 2, aggregation * 2);
    }
}

void isSorted()
{
    int max = 0;
    for (int i = 1; i < ARR_LENGTH; i++)
    {
        if (arr[i] >= arr[i - 1])
        {
            max = arr[i];
        }
        else
        {
            printf("== Unsorted Array ==\n");
            return;
        }
    }
    printf("== Sorted Array ==\n");
}

int main()
{
    srand(RANDOM_SEED);
    int number_of_runs = 20;
    int time_sum = 0;

    long long results[number_of_runs];

    arr = (int *)malloc(ARR_LENGTH * sizeof(int));
    arr_copy = (int *)malloc(ARR_LENGTH * sizeof(int));

    for (int i = 0; i < number_of_runs; i++)
    {
        for (int i = 0; i < ARR_LENGTH; i++)
        {
            arr[i] = LOWER_NUMBER_LIMIT + (UPPER_NUMBER_LIMIT - LOWER_NUMBER_LIMIT) * ((double)rand() / RAND_MAX);
            arr_copy[i] = arr[i];
        }

        if (VERBOSE)
        {
            printf("Starting parallel merge sort\n");
            isSorted();
        }

        pthread_t threads[NUM_THREADS];
        long long parallel_start = timeInMilliseconds();
        for (long i = 0; i < NUM_THREADS; i++)
        {
            int retval = pthread_create(&threads[i], NULL, mergeSortThreaded, (void *)i);
            if (retval)
            {
                printf("ERROR -> %d\n", retval);
                exit(-1);
            }
        }

        for (int i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(threads[i], NULL);
        }

        mergeChunks(NUM_THREADS, 1);
        long long parallel_end = timeInMilliseconds();
        results[i] = parallel_end - parallel_start;
        if (VERBOSE)
        {
            printf("Time Elapsed for parallel in milliseconds: %lld ms\n", parallel_end - parallel_start);
            isSorted();
        }
    }

    for (int i = 0; i < number_of_runs; i++)
    {
        time_sum += results[i];
    }

    printf("Parallel: Array size= %d | Number of runs= %d | Number of threads= %d | Average time= %d ms\n", ARR_LENGTH, number_of_runs, NUM_THREADS, time_sum / number_of_runs);

    for (int i = 0; i < number_of_runs; i++)
    {
        // Replace arr with arr_copy to ensure that both expirements have the same elements (fair comparison)
        for (int i = 0; i < ARR_LENGTH; i++)
        {
            arr[i] = arr_copy[i];
        }

        if (VERBOSE)
        {
            printf("Starting sequential merge sort\n");
            isSorted();
        }

        long long sequential_start = timeInMilliseconds();
        mergeSort(0, ARR_LENGTH - 1);
        long long sequential_end = timeInMilliseconds();
        results[i] = sequential_end - sequential_start;
        if (VERBOSE)
        {
            printf("Time Elapsed for sequential in milliseconds: %lld ms\n", sequential_end - sequential_start);
            isSorted();
        }
    }

    time_sum = 0;
    for (int i = 0; i < number_of_runs; i++)
    {
        time_sum += results[i];
    }

    printf("Sequential: Array size= %d | Number of runs= %d | Average time= %d ms\n", ARR_LENGTH, number_of_runs, time_sum / number_of_runs);

    return 0;
}
