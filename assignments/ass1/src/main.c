#include "headers.h"
#include "utils.c"

int main(int argc, char **argv)
{

    printf("you have %d cores on your machine\n", getNumCPUs());

    if (argc<2){
        printf("provide a folder name after the command to test file listing\n");
        exit(0);
    }

    // we supose that the program take a command line parameter to pass folder name containing data files
    char **filesList = listFiles(argv[1]);

    // here we are just listing files name, the list is terminated with a NULL marker
    while (*filesList != NULL)
    {
        printf("%s\n", *filesList);
        filesList++;
    }

    return 0;
}