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
    char *command = malloc(sizeof(char) * 50);
    strcpy(command, "bash -c \"./clean_dups_from_zipped.sh ");
    strcat(command, folder_name);
    strcat(command, "\"");
    system(command);
    free(command);
    printf("Cleaning up...\n");
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

int parallelRunMethod1(char **filesList, int numberOfRuns){
    time_t start, end;

    start = time(NULL);

    int pid;
    while (*filesList != NULL)
    {
        pid = fork();
        if (pid == 0)
        {
            compressFile(*filesList);
            exit(0);
        }
        filesList++;
    }

    while (wait(NULL) > 0)
        ;

    end = time(NULL);

    return (int) (end - start);
}

int parallelRunMethod2(char **filesList, int numberOfRuns){
    time_t start, end;

    start = time(NULL);

    int pid;
    int cores = getNumCPUs();
    int filesPerCore = numberOfRuns / cores;
    int filesLeft = numberOfRuns % cores;
    int filesToRun = filesPerCore;
    while (*filesList != NULL)
    {
        pid = fork();
        if (pid == 0)
        {
            compressFile(*filesList);
            exit(0);
        }
        filesList++;
        filesToRun--;
        if (filesToRun == 0)
        {
            if (filesLeft > 0)
            {
                filesLeft--;
                filesToRun = filesPerCore + 1;
            }
            else
            {
                filesToRun = filesPerCore;
            }
            while (wait(NULL) > 0)
                ;
        }
    }

    while (wait(NULL) > 0)
        ;

    end = time(NULL);

    return (int) (end - start);
}

int parallelRunMethod3(char **filesList, int numberOfRuns){
    time_t start, end;

    start = time(NULL);

    int pid;
    int cores = getNumCPUs();
    int filesPerCore = numberOfRuns / cores;
    int filesLeft = numberOfRuns % cores;
    int filesToRun = filesPerCore;
    while (*filesList != NULL)
    {
        pid = fork();
        if (pid == 0)
        {
            compressFile(*filesList);
            exit(0);
        }
        filesList++;
        filesToRun--;
        if (filesToRun == 0)
        {
            if (filesLeft > 0)
            {
                filesLeft--;
                filesToRun = filesPerCore + 1;
            }
            else
            {
                filesToRun = filesPerCore;
            }
        }
    }

    while (wait(NULL) > 0)
        ;

    end = time(NULL);

    return (int) (end - start);
}

double parallel(char **filesList, int numberOfRuns, int strategy){

    int *runs = malloc(sizeof(int) * numberOfRuns);

    // Choose the strategy
    int (*parallelRunMethod)(char **, int) = NULL;
    switch (strategy)
    {
    case 1:
        parallelRunMethod = &parallelRunMethod1;
        break;
    case 2:
        parallelRunMethod = &parallelRunMethod2;
        break;
    case 3:
        parallelRunMethod = &parallelRunMethod3;
        break;
    default:
        printf("Invalid strategy number\n");
        exit(0);
        break;
    }



    for (int i = 0; i < numberOfRuns; i++)
    {   
        runs[i] = parallelRunMethod(filesList, numberOfRuns);
        printf("Parallel Strategy %d run %d took %d seconds\n", strategy,  i, runs[i]);
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

double parallelWithCoresRun(char **filesList, int cores, int numberOfFiles) {
    time_t start = time(NULL);

    int filesPerCore = numberOfFiles / cores;
    int remainingFiles = numberOfFiles % cores;

    int *filesPerCoreList = malloc(sizeof(int) * cores);
    for (int i = 0; i < cores; i++) {
        filesPerCoreList[i] = filesPerCore;
    }

    // Loop circularly through the cores and add the remaining files
    int i = 0;
    while (remainingFiles > 0) {
        filesPerCoreList[i]++;
        remainingFiles--;
        i++;
        if (i == cores) {
            i = 0;
        }
    }
    

    for (int i = 0; i < cores; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) { // Child process
            int startIdx = 0;
            for (int j = 0; j < i; j++) {
                startIdx += filesPerCoreList[j];
            }
            int endIdx = startIdx + filesPerCoreList[i];

            for (int j = startIdx; j < endIdx; j++) {
                compressFile(filesList[j]);
            }

            exit(0);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < cores; i++) {
        wait(NULL);
    }

    time_t end = time(NULL);

    return (int) (end - start);
}

double parallelWithCores(char **filesList, int numberOfRuns, int cores, int numberOfFiles) {

    int *runs = malloc(sizeof(int) * numberOfRuns);

    for (int i = 0; i < numberOfRuns; i++) {
        runs[i] = parallelWithCoresRun(filesList, cores, numberOfFiles);
    }

    double avg = 0;
    for (int i = 0; i < numberOfRuns; i++) {
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
    int numberOfFiles = 0;
    while (filesList[numberOfFiles] != NULL)
    {
        numberOfFiles++;
    }


    // Run each expirement numberOfRuns times, and get the average time
    int numberOfRuns = 5;

    // printf("Starting sequential compression %d times\n", numberOfRuns);
    // double sequentialTime = sequential(filesList, numberOfRuns);
    // printf("Avg. Sequential time after %d runs: %f\n", numberOfRuns, sequentialTime);
    // clean_up(argv[1]);


    // For each number of cores, run the parallel compression numberOfRuns times, and get the average time
    for (int i = 2; i <= getNumCPUs(); i++)
    {
        printf("Starting parallel compression with  %d cores %d times\n", i, numberOfRuns);
        double parallelTime = parallelWithCores(filesList, numberOfRuns, i, numberOfFiles);
        printf("Avg. Parallel time after %d runs: %f\n", numberOfRuns, parallelTime);
        clean_up(argv[1]);
    }


    // Strategy 1 - Create N parallel tasks at same time to proccess the N files
    // Strategy 2 - Create NB_CORES tasks each time to proccess NB_CORES files, when all the files are proccessed, create another NB_CORES taks to proccess the next NB_CORES files, and so on until all the files are proccessed.
    // Strategy 3 - Create NB_CORES tasks at all, and assign multiple files compression for each task (as equal as possible)
    // int strategy = 1;
    // printf("Starting strategy %d parallel compression %d times\n", strategy, numberOfRuns);
    // double parallelTime = parallel(filesList, numberOfRuns, strategy);
    // printf("Avg. Parallel time after %d runs: %f\n", numberOfRuns, parallelTime);
    // clean_up(argv[1]);

    return 0;
}