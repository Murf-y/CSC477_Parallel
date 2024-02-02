// Parallelize the counting of value "3" occurence in a very very big array of integers. Use 2 threads. The first thread will count the occurence of "3" in the first half of the array, the second thread will count the occurence of "3" in the second half of the array. The main thread will wait for the 2 threads to finish and then display the total occurence of "3" in the array.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 100000000
#define NUM_THREADS 2

void *thread_function(void *arg);

struct thread_args
{
    int *array;
    int start;
    int end;
    int rank;
};

int main()
{
    int *array = malloc(sizeof(int) * ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = rand() % 10;
    }

    pthread_t threads[NUM_THREADS];

    int *occurences = malloc(sizeof(int) * NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        occurences[i] = 0;
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        struct thread_args *args = malloc(sizeof(struct thread_args));
        args->array = array;
        args->start = i * (ARRAY_SIZE / NUM_THREADS);
        args->end = (i + 1) * (ARRAY_SIZE / NUM_THREADS);
        args->rank = i;

        pthread_create(&threads[i], NULL, thread_function, args);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], (void **)&occurences[i]);
    }

    int totalOccurences = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        totalOccurences += occurences[i];
    }

    printf("Total occurences of 3: %d\n", totalOccurences);

    free(array);

    return 0;
}

void *thread_function(void *arg)
{
    struct thread_args *args = (struct thread_args *)arg;

    int occurences = 0;

    for (int i = args->start; i < args->end; i++)
    {
        if (args->array[i] == 3)
        {
            occurences++;
        }
    }

    pthread_exit((void *)occurences);
}
