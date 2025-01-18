#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"

/* Determine file separator based on the platform */
#ifdef _WIN32
    #define FILE_SEPARATOR '\\'
    #include <direct.h>  // For _mkdir in Windows
#else
#define FILE_SEPARATOR '/'
#endif


/* Function to check if a path exists (file or directory) */
bool utils_path_exists(const char* path)
{
    struct stat stat_buf;

    // Use stat to get information about the file or directory
    if (stat(path, &stat_buf) == 0)
    {
        return true; // The path exists (whether file or directory)
    }

    return false; // The path does not exist
}


/* Method to check if a directory exists */
bool utils_directory_exists(const char* path)
{
    struct stat stat_buf;

    // Use stat to get information about the file or directory
    if (stat(path, &stat_buf) == 0)
    {
        // Check if the path is a directory
        if (S_ISDIR(stat_buf.st_mode))
        {
            return true;
        }
    }

    // Return false if the directory doesn't exist or isn't a directory
    return false;
}


/* Helper function to join repo.codesync_directory with additional paths */
char* utils_join_paths(const char* base, const char* path)
{
    const size_t base_len = strlen(base);
    const size_t path_len = strlen(path);
    char* result = malloc(base_len + path_len + 2); // For separator and null terminator

    if (result == NULL)
    {
        return nullptr;
    }

    strcpy(result, base);
    if (base[base_len - 1] != FILE_SEPARATOR)
    {
        result[base_len] = FILE_SEPARATOR;
        strcpy(result + base_len + 1, path);
    }
    else
    {
        strcpy(result + base_len, path);
    }

    return result;
}



/* Computes the path under repo's codesync directory */
char* utils_repo_path(const Repository* repo, const char* path, ...)
{
    va_list args;
    va_start(args, path);

    char* full_path = strdup(repo->codesync_directory);
    if (full_path == NULL)
    {
        va_end(args);
        return nullptr;
    }

    char* next_path;
    while ((next_path = va_arg(args, char *)) != NULL)
    {
        char* tmp = utils_join_paths(full_path, next_path);
        free(full_path);
        full_path = tmp;
        if (full_path == NULL)
        {
            va_end(args);
            return nullptr;
        }
    }

    va_end(args);
    return full_path;
}



/* Computes the path under repo's gitdir and creates missing directories */
char* utils_repo_file(const Repository* repo, const char* path, const bool mkdir_flag)
{
    const char* dir = utils_repo_dir(repo, path, mkdir_flag);
    if (dir != NULL)
    {
        return utils_repo_path(repo, path);
    }
    return nullptr;
}
