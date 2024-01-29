// This files contains functions useful for assignment program : parallel compress tool
// ------------------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int getNumCPUs()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int countFiles(const char *folderPath)
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    dir = opendir(folderPath);
    if (dir == NULL)
    {
        return -1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
            count++;
    }
    closedir(dir);
    return count;
}

char **listFiles(const char *folderPath)
{
    DIR *dir;
    struct dirent *entry;
    char **filesList;
    int filesCount = countFiles(folderPath);

    if (filesCount == -1)
    {
        filesList = (char **)malloc(sizeof(char *));
        *filesList = NULL;
        printf("Error opening directory <%s>\n", folderPath);
        return filesList;
    }

    dir = opendir(folderPath);

    filesList = (char **)malloc((filesCount + 1) * sizeof(char *));
    int fileIndex = 0;
    char *full_file_name;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            full_file_name = malloc((strlen(folderPath) + strlen(entry->d_name) + 5) * sizeof(char));
            sprintf(full_file_name, "\"%s/%s\"", folderPath, entry->d_name);
            filesList[fileIndex++] = full_file_name;
        }
    }
    filesList[fileIndex] = NULL;
    closedir(dir);
    return filesList;
}