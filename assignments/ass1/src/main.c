#include "headers.h"
#include "utils.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

/*
Assignment Instructions:

Parallelize the compression of N files using multiple methods:

1. Creating N parallel tasks at same time to proccess the N files

2. Creating NB_CORES tasks each time to proccess NB_CORES files, when all the files are proccessed, create another NB_CORES taks to proccess the next NB_CORES files, and so on until all the files are proccessed.

3. Creating NB_CORES tasks at all, and assign multiple files compression for each task (as equal as possible)


Performance Analysis:
    - Investigate the impact of your strategies on the performance.
    - Start with comparison of sequential vs 2, 3, 4 .. NB_CORES parallelism. Then with the 3 mentioned strategies.
    - Draw and excel graph to show scale up of performance with the number of cores.

*/

void compressFile(char *fileName)
{   
    // Command cannot be longer than 50 characters raw_command = 10, fileName = 20, compressedFileName = 20
    
    char *command = malloc(sizeof(char) * 50);
    strcpy(command, "gzip -k -f -1 ");
    strcat(command, fileName);
    system(command);
    free(command);
}

void clean_up(char *folder_name)
{
    // We have to remove already zipped files to not affect the next runs
    char *command = malloc(sizeof(char) * 50);
    strcpy(command, "./clean_dups_from_zipped.sh ");
    strcat(command, folder_name);
    printf("Cleaning up...\n");
    execlp("bash", "bash", "-c", command, NULL);
}

int sequentialRun(char **filesList){
    time_t start, end;

    start = time(NULL);

    while (*filesList != NULL)
    {
        compressFile(*filesList);
        filesList++;
    }

    end = time(NULL);

    return (int) (end - start);
}

double sequential(char **filesList, int numberOfRuns){

    int *runs = malloc(sizeof(int) * numberOfRuns);

    for (int i = 0; i < numberOfRuns; i++)
    {
        runs[i] = sequentialRun(filesList);
        printf("Sequential run %d took %d seconds\n", i, runs[i]);
    }
    
    double avg = 0;
    for (int i = 0; i < numberOfRuns; i++)
    {
        avg += runs[i];
    }
    free(runs);
    
    avg = avg / numberOfRuns;
    return avg;

}


int main(int argc, char **argv)
{

    printf("You have %d cores on your machine\n", getNumCPUs());

    if (argc<2){
        printf("Provide a folder name after the command to test file listing\n");
        exit(0);
    }

    // we supose that the program take a command line parameter to pass folder name containing data files
    char **filesList = listFiles(argv[1]);

    // Run each expirement numberOfRuns times, and get the average time
    int numberOfRuns = 5;

    // printf("Starting sequential compression %d times\n", numberOfRuns);
    // double sequentialTime = sequential(filesList, numberOfRuns);
    // printf("Avg. Sequential time after %d runs: %f\n", numberOfRuns, sequentialTime);
    // clean_up(argv[1]);

    

    return 0;
}