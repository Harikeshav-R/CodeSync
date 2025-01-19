#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


/**
 * Check if a path exists (file or directory).
 *
 * @param path The path to check.
 * @return True if the path exists, false otherwise.
 */
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


/**
 * Check if a directory exists at the given path.
 *
 * @param path The directory path to check.
 * @return True if the path is a directory, false otherwise.
 */
bool utils_directory_exists(const char* path)
{
    struct stat stat_buf;

    // Use stat to get information about the file or directory
    if (stat(path, &stat_buf) == 0)
    {
        // Check if the path is a directory
        if (S_ISDIR(stat_buf.st_mode))
        {
            return true; // The path is a directory
        }
    }

    // Return false if the directory doesn't exist or isn't a directory
    return false;
}


/**
 * Join two paths into one. If the base path doesn't end with a separator, one will be added.
 *
 * @param base The base path.
 * @param path The path to join to the base.
 * @return A newly allocated string containing the full path, or NULL if memory allocation fails.
 */
char* utils_join_paths(const char* base, const char* path)
{
    const size_t base_len = strlen(base);
    const size_t path_len = strlen(path);
    char* result = malloc(base_len + path_len + 2); // For separator and null terminator

    if (result == NULL)
    {
        return nullptr; // Return nullptr if memory allocation fails
    }

    strcpy(result, base); // Copy the base path
    if (base[base_len - 1] != FILE_SEPARATOR)
    {
        result[base_len] = FILE_SEPARATOR; // Add separator if not present
        strcpy(result + base_len + 1, path); // Append the path
    }
    else
    {
        strcpy(result + base_len, path); // Append the path directly if separator is present
    }

    return result; // Return the combined path
}


/**
 * Build the full path under a repository directory using variable arguments.
 *
 * @param repository The repository structure.
 * @param count The number of arguments following the repository.
 * @param args The variable arguments containing path components.
 * @return A newly allocated string containing the full path, or NULL if memory allocation fails.
 */
char* utils_repo_path(const Repository* repository, const int count, va_list args)
{
    // Start with the base path (codesync directory)
    char* full_path = strdup(repository->codesync_directory);
    if (!full_path)
    {
        perror("strdup"); // Handle memory allocation error
        return nullptr;
    }

    // Iterate over the variable arguments to build the full path
    for (int i = 0; i < count; i++)
    {
        const char* component = va_arg(args, const char*);
        char* temp_path = utils_join_paths(full_path, component);
        if (!temp_path)
        {
            free(full_path); // Free memory if joining fails
            return nullptr;
        }

        free(full_path); // Free the old path
        full_path = temp_path; // Update the full path
    }

    return full_path; // Return the final computed path
}


/**
 * Compute the repository directory path and create missing directories if requested.
 *
 * @param repository The repository structure.
 * @param mkdir_flag If true, missing directories will be created.
 * @param count The number of path components.
 * @param args The variable arguments containing path components.
 * @return A newly allocated string containing the directory path, or NULL if there is an error.
 */
static char* utils_repo_dir_va(const Repository* repository, const bool mkdir_flag, const int count, va_list args)
{
    // Generate the full path by calling utils_repo_path
    char* full_path = utils_repo_path(repository, count, args);
    if (!full_path)
    {
        return nullptr; // Return NULL if path creation failed
    }

    struct stat stat_buf;
    // Check if the path exists and is a directory
    if (stat(full_path, &stat_buf) == 0)
    {
        if (S_ISDIR(stat_buf.st_mode))
        {
            return full_path; // Directory exists, return path
        }

        free(full_path); // Path exists but is not a directory
        return nullptr;
    }

    // If directory doesn't exist and mkdir_flag is set, try to create it
    if (mkdir_flag)
    {
        const int result = utils_make_dirs(full_path);
        if (result == 0 || errno == EEXIST)
        {
            return full_path; // Directory created or already exists
        }

        free(full_path); // Failed to create directory
        return nullptr;
    }

    free(full_path); // Directory doesn't exist, return NULL
    return nullptr;
}


/**
 * Compute the repository directory path with variable arguments and create missing directories if requested.
 *
 * @param repository The repository structure.
 * @param mkdir_flag If true, missing directories will be created.
 * @param count The number of path components.
 * @param ... The variable arguments containing path components.
 * @return A newly allocated string containing the directory path, or NULL if there is an error.
 */
char* utils_repo_dir(const Repository* repository, const bool mkdir_flag, const int count, ...)
{
    va_list args;
    va_start(args, count);

    // Call helper function with the arguments to compute the directory
    char* result = utils_repo_dir_va(repository, mkdir_flag, count, args);

    va_end(args);
    return result; // Return the computed directory path
}
