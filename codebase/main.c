#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc, char **argv)
{
    // Dynamic memory allocation of 2D array

    int **arr = (int **)malloc(3 * sizeof(int *));
    for (int i = 0; i < 3; i++)
        arr[i] = (int *)malloc(3 * sizeof(int));

    // Fill it with 1

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3;j++)
            arr[i][j] = 1;

    // Print the array in matrix form

    for (int i = 0; i < 3; i++)
    {
        printf("\n");
        for (int j = 0; j < 3;j++)
            printf("%d ", *arr);
    }
}