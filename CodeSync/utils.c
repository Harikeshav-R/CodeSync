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

