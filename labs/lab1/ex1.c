// Write a c program two threads each one display hi rank (related to the creation order) we want to define one unique function for both threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg);

int main()
{
    pthread_t threads[2];

    for (int i = 0; i < 2; i++)
    {
        pthread_create(&threads[i], NULL, thread_function, &i);
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *thread_function(void *arg)
{
    int *rankPtr = (int *)arg;
    printf("Hi my rank is: %d\n", *rankPtr);
    pthread_exit(NULL);
}
