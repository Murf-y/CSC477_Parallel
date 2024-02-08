
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

void *thread_function(void *arg);
int timeInMilliseconds();

// use a mutex for the counter to be shared between threads
pthread_mutex_t mutex;

int counter = 0;

int main()
{
    // initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    pthread_t thread1, thread2;
    int ret1, ret2;

    int startTime = timeInMilliseconds();

    ret1 = pthread_create(&thread1, NULL, thread_function, NULL);
    ret2 = pthread_create(&thread2, NULL, thread_function, NULL);

    if (ret1 != 0 || ret2 != 0)
    {
        printf("Error creating threads\n");
        exit(1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    int endTime = timeInMilliseconds();

    printf("Final value of counter: %d\n", counter);
    printf("Time taken: %dms\n", endTime - startTime);

    return 0;
}

void *thread_function(void *arg)
{
    for (int i = 0; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int timeInMilliseconds()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}
