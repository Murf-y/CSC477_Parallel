#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#define N 100000
#define N_THREADS 4
#define MIN_ROW 10
#define MAX_ROW 10000

int **matrix;
int *primeCounter;
pthread_mutex_t mutex;

// Global job counter for dynamic balancing
int jobCounter = 0;

long long timeInMilliseconds(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

int is_prime(int n)
{
    if (n <= 1)
    {
        return 0;
    }
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

void *static_balancing_thread(void *arg)
{
    int thread_id = *(int *)arg;
    int start_index = (N / N_THREADS) * thread_id;
    int end_index = start_index + (N / N_THREADS) - 1;

    if (thread_id == N_THREADS - 1)
    {
        end_index = N - 1;
    }

    for (int i = start_index; i <= end_index; i++)
    {
        int j = 0;
        int count = 0;
        while (matrix[i][j] != -1)
        {
            if (is_prime(matrix[i][j]))
            {
                count++;
            }
            j++;
        }
        primeCounter[i] = count;
    }
    return NULL;
}

void *dynamic_balancing_thread(void *arg)
{
    while (1)
    {
        int i;
        pthread_mutex_lock(&mutex);
        i = jobCounter;
        jobCounter++;
        pthread_mutex_unlock(&mutex);

        if (i >= N)
        {
            break;
        }

        int j = 0;
        int count = 0;
        while (matrix[i][j] != -1)
        {
            if (is_prime(matrix[i][j]))
            {
                count++;
            }
            j++;
        }
        primeCounter[i] = count;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    // pthread_mutex_init(&mutex, NULL);
    matrix = malloc(N * sizeof(int *));
    primeCounter = malloc(N * sizeof(int));

    for (int i = 0; i < N; i++)
    {
        // Calculate a random row size
        int rowSize = (rand() % (MAX_ROW - MIN_ROW + 1)) + MIN_ROW;
        matrix[i] = malloc(rowSize * sizeof(int));
        primeCounter[i] = 0;
        for (int j = 0; j < rowSize - 1; j++)
        {
            matrix[i][j] = rand() % 1000;
        }

        // To indicate the end of the row
        matrix[i][rowSize - 1] = -1;
    }

    // Static balancing (measure the time)

    pthread_t threads[N_THREADS];
    int thread_ids[N_THREADS];

    long long start = timeInMilliseconds();
    for (int i = 0; i < N_THREADS; i++)
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, static_balancing_thread, &thread_ids[i]);
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    long long end = timeInMilliseconds();
    printf("Static balancing took %lld milliseconds\n", end - start);

    // Dynamic balancing (measure the time)
    start = timeInMilliseconds();
    jobCounter = 0;
    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, dynamic_balancing_thread, NULL);
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    end = timeInMilliseconds();
    printf("Dynamic balancing took %lld milliseconds\n", end - start);
}