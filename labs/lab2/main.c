#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#define N 100000000
#define NT 16

int array[N];
int thread_counts[NT]; // Global array for thread counts in Case 4
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int global_count = 0;

void case1(int *array, long long size)
{
    long long count = 0;
    for (long long i = 0; i < size; i++)
    {
        if (array[i] == 3)
        {
            count++;
        }
    }
}

void *count3s_case2(void *arg)
{
    int thread_part = *((int *)arg);
    int start = thread_part * (N / NT);
    int end = (thread_part + 1) * (N / NT);

    for (int i = start; i < end; i++)
    {
        if (array[i] == 3)
        {
            pthread_mutex_lock(&mutex);
            global_count++; // Counting is commented out as you only want time
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

void case2()
{
    pthread_t threads[NT];
    int thread_args[NT];

    for (int i = 0; i < NT; i++)
    {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, count3s_case2, (void *)&thread_args[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }
    for (int i = 0; i < NT; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            fprintf(stderr, "Error joining thread\n");
            exit(2);
        }
    }
}

void *count3s_case3(void *arg)
{
    int thread_part = *((int *)arg);
    int start = thread_part * (N / NT);
    int end = (thread_part + 1) * (N / NT);
    int local_count = 0; // Local counter for each thread

    for (int i = start; i < end; i++)
    {
        if (array[i] == 3)
        {
            local_count++;
        }
    }
    return (void *)(intptr_t)local_count;
}

void case3()
{
    pthread_t threads[NT];
    int thread_args[NT];

    for (int i = 0; i < NT; i++)
    {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, count3s_case3, (void *)&thread_args[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    int total_count = 0;
    for (int i = 0; i < NT; i++)
    {
        void *local_count;
        if (pthread_join(threads[i], &local_count))
        {
            fprintf(stderr, "Error joining thread\n");
            exit(2);
        }
        // Local counts are not summed as you only want time
        total_count += (intptr_t)local_count;
    }
}

void *count3s_case4(void *arg)
{
    int thread_part = *((int *)arg);
    int start = thread_part * (N / NT);
    int end = (thread_part + 1) * (N / NT);

    for (int i = start; i < end; i++)
    {
        if (array[i] == 3)
        {
            thread_counts[thread_part]++;
        }
    }
    return NULL;
}

void case4()
{
    pthread_t threads[NT];
    int thread_args[NT];

    // Initialize thread_counts to 0
    for (int i = 0; i < NT; i++)
    {
        thread_counts[i] = 0;
    }

    for (int i = 0; i < NT; i++)
    {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, count3s_case4, (void *)&thread_args[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < NT; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            fprintf(stderr, "Error joining thread\n");
            exit(2);
        }
    }

    // Summing the counts from all threads
    int total_count = 0;
    for (int i = 0; i < NT; i++)
    {
        total_count += thread_counts[i];
    }
}

void initializeArray(int *array, long long size)
{
    for (long long i = 0; i < size; i++)
    {
        array[i] = rand() % 10;
    }
}

long long getTimeInMilliseconds(struct timeval start, struct timeval end)
{
    long seconds = end.tv_sec - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    return ((seconds) * 1000 + useconds / 1000.0) + 0.5;
}

void case3WithCustomNT(int nt)
{
    pthread_t threads[nt];
    int thread_args[nt];

    for (int i = 0; i < nt; i++)
    {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, count3s_case3, (void *)&thread_args[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    int total_count = 0;
    for (int i = 0; i < nt; i++)
    {
        void *local_count;
        if (pthread_join(threads[i], &local_count))
        {
            fprintf(stderr, "Error joining thread\n");
            exit(2);
        }
        total_count += (intptr_t)local_count;
        // Local counts are not summed as you only want time
    }
}

int main()
{
    struct timeval start, end;
    long long elapsedTime;

    // Initialize array
    initializeArray(array, N);

    // Case 1: Sequential counting
    gettimeofday(&start, NULL);
    case1(array, N);
    gettimeofday(&end, NULL);
    elapsedTime = getTimeInMilliseconds(start, end);
    printf("Case 1 - Time taken: %lld milliseconds.\n", elapsedTime);

    // Case 2: Parallel counting with global shared counter
    gettimeofday(&start, NULL);
    case2();
    gettimeofday(&end, NULL);
    elapsedTime = getTimeInMilliseconds(start, end);
    printf("Case 2 - Time taken: %lld milliseconds.\n", elapsedTime);

    // Case 3: Parallel counting with local counters
    gettimeofday(&start, NULL);
    case3();
    gettimeofday(&end, NULL);
    elapsedTime = getTimeInMilliseconds(start, end);
    printf("Case 3 - Time taken: %lld milliseconds.\n", elapsedTime);

    // Case 4: Parallel counting with private counters
    // This case will be slow because of: False sharing
    // This means that the threads are writing to the same cache line
    // This is because the array is contiguous in memory, and the threads are writing to the same cache line, causing cache invalidation, and the cache line to be written back to memory
    gettimeofday(&start, NULL);
    case4();
    gettimeofday(&end, NULL);
    elapsedTime = getTimeInMilliseconds(start, end);
    printf("Case 4 - Time taken: %lld milliseconds.\n", elapsedTime);

    // Case 3 with custom NT, 1, 2, 4, 8, 16
    for (int i = 1; i <= 16; i *= 2)
    {
        gettimeofday(&start, NULL);
        case3WithCustomNT(i);
        gettimeofday(&end, NULL);
        elapsedTime = getTimeInMilliseconds(start, end);
        printf("Case 3 with NT = %d - Time taken: %lld milliseconds.\n", i, elapsedTime);
    }

    // Clean up
    pthread_mutex_destroy(&mutex);
    return 0;
}