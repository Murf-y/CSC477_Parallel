#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Validate that the file exists
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    // Keep track of time
    time_t start_time = time(NULL);

    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child process
        printf("Child process ID: %d\n", getpid());

        // use system call to compress the file
        char command[100];
        strcpy(command, "gzip -k -f -S .child.gz ");

        // append the filename to the command
        strcat(command, argv[1]);

        // execute the command
        system(command);


        time_t end_time = time(NULL);
        printf("Time taken for child process: %ld seconds\n", end_time - start_time);
    
    } else {
        // Parent process
        printf("Parent process ID: %d\n", getpid());

        // Wait for the child process to complete
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            fprintf(stderr, "Child process exited abnormally\n");
        }

        // use system call to compress the file
        char command[100];
        strcpy(command, "gzip -k -f -S .parent.gz ");

        // append the filename to the command
        strcat(command, argv[1]);

        // execute the command
        system(command);


        time_t end_time = time(NULL);
        printf("Time taken for parent process: %ld seconds\n", end_time - start_time);
    }

    time_t end_time = time(NULL);
    printf("Time taken for both processes: %ld seconds\n", end_time - start_time);

    return 0;
}
