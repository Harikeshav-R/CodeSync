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


/**
 * Compute the repository file path and create any required directories if requested.
 *
 * @param repository The repository structure.
 * @param mkdir_flag If true, missing directories will be created.
 * @param count The number of path components.
 * @param ... The variable arguments containing path components.
 * @return A newly allocated string containing the file path, or NULL if there is an error.
 */
char* utils_repo_file(const Repository* repository, const bool mkdir_flag, const int count, ...)
{
    va_list args;
    va_start(args, count);

    va_list args_copy;
    va_copy(args_copy, args); // Create a copy of args for reuse

    // Get the directory part of the path
    char* dir = utils_repo_dir_va(repository, mkdir_flag, count - 1, args);
    if (!dir)
    {
        va_end(args);
        va_end(args_copy);
        return nullptr; // Return NULL if directory creation failed
    }

    // Get the full file path
    char* file_path = utils_repo_path(repository, count, args_copy);
    va_end(args);
    va_end(args_copy);

    free(dir); // Free directory path as it is no longer needed
    return file_path; // Return the final file path
}


/**
 * Check if a directory is empty.
 *
 * @param path The directory path to check.
 * @return True if the directory is empty, false if it contains files, or -1 in case of an error.
 */
bool utils_is_directory_empty(const char* path)
{
#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", path);

    // Use FindFirstFile to search for files in the directory
    hFind = FindFirstFile(search_path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("FindFirstFile");
        return -1; // Error opening directory
    }

    do {
        // Skip the "." and ".." entries
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            FindClose(hFind);
            return false; // Directory is not empty
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    return true; // Directory is empty

#else
    DIR* dir = opendir(path);
    if (!dir)
    {
        return -1; // Error opening directory
    }

    struct dirent* entry;
    // Loop through directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            closedir(dir);
            return false; // Directory is not empty
        }
    }

    closedir(dir);
    return true; // Directory is empty
#endif
}


/**
 * Create directories along the specified path.
 *
 * @param path The path to create.
 * @return 0 on success, -1 if an error occurred.
 */
int utils_make_dirs(const char* path)
{
    char temp_path[1024];
    char* p = nullptr;

    // Check if path is NULL or empty
    if (path == NULL || strlen(path) == 0)
    {
        return -1; // Invalid path
    }

    // Create a copy of the path to manipulate
    strncpy(temp_path, path, sizeof(temp_path));
    temp_path[sizeof(temp_path) - 1] = '\0';

    size_t len = strlen(temp_path);

#ifdef _WIN32
    // Windows specific: Create each directory in the path
    if (temp_path[len - 1] == '\\') {
        temp_path[len - 1] = '\0'; // Remove trailing backslash
    }

    // Iterate over path characters to create directories
    for (p = temp_path + 1; *p; p++) {
        if (*p == '\\') {
            *p = '\0'; // Temporarily terminate the string to create directories
            if (CreateDirectory(temp_path, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS) {
                perror("CreateDirectory");
                return -1; // Failed to create directory
            }
            *p = '\\'; // Restore the backslash to continue
        }
    }

    // Create the final directory
    if (CreateDirectory(temp_path, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS) {
        perror("CreateDirectory");
        return -1; // Failed to create the final directory
    }

#else
    // POSIX (Linux/macOS) specific: Create each directory in the path
    if (temp_path[len - 1] == '/')
    {
        temp_path[len - 1] = '\0'; // Remove trailing slash
    }

    for (p = temp_path + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = '\0'; // Temporarily terminate the string to create directories
            if (mkdir(temp_path, S_IRWXU) != 0 && errno != EEXIST)
            {
                perror("mkdir");
                return -1; // Failed to create directory
            }
            *p = '/'; // Restore the slash to continue
        }
    }

    // Create the final directory
    if (mkdir(temp_path, S_IRWXU) != 0 && errno != EEXIST)
    {
        perror("mkdir");
        return -1; // Failed to create the final directory
    }

#endif

    return 0; // Success
}
