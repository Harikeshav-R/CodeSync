//
// Created by Harikeshav R on 1/18/25.
//


#ifndef UTILS_H
#define UTILS_H

/* Determine file separator based on the platform */
#ifdef _WIN32
#define FILE_SEPARATOR '\\'
#include <direct.h>  // For _mkdir in Windows
#include <windows.h>
#else
#define FILE_SEPARATOR '/'
#include <dirent.h>
#endif

#include "repository.h"


/**
 * Check if a path exists (file or directory).
 *
 * @param path The path to check.
 * @return True if the path exists, false otherwise.
 */
bool utils_path_exists(const char* path);


/**
 * Check if a directory exists at the given path.
 *
 * @param path The directory path to check.
 * @return True if the path is a directory, false otherwise.
 */
bool utils_directory_exists(const char* path);


/**
 * Join two paths into one. If the base path doesn't end with a separator, one will be added.
 *
 * @param base The base path.
 * @param path The path to join to the base.
 * @return A newly allocated string containing the full path, or NULL if memory allocation fails.
 */
char* utils_join_paths(const char* base, const char* path);


/**
 * Build the full path under a repository directory using variable arguments.
 *
 * @param repository The repository structure.
 * @param count The number of arguments following the repository.
 * @param args The variable arguments containing path components.
 * @return A newly allocated string containing the full path, or NULL if memory allocation fails.
 */
char* utils_repo_path(const Repository* repository, int count, va_list args);


/**
 * Compute the repository directory path and create missing directories if requested.
 *
 * @param repository The repository structure.
 * @param mkdir_flag If true, missing directories will be created.
 * @param count The number of path components.
 * @param args The variable arguments containing path components.
 * @return A newly allocated string containing the directory path, or NULL if there is an error.
 */
char* utils_repo_dir(const Repository* repository, bool mkdir_flag, int count, ...);


/**
 * Compute the repository directory path with variable arguments and create missing directories if requested.
 *
 * @param repository The repository structure.
 * @param mkdir_flag If true, missing directories will be created.
 * @param count The number of path components.
 * @param ... The variable arguments containing path components.
 * @return A newly allocated string containing the directory path, or NULL if there is an error.
 */
char* utils_repo_file(const Repository* repository, bool mkdir_flag, int count, ...);


/**
 * Check if a directory is empty.
 *
 * @param path The directory path to check.
 * @return True if the directory is empty, false if it contains files, or -1 in case of an error.
 */
bool utils_is_directory_empty(const char* path);


/**
 * Create directories along the specified path.
 *
 * @param path The path to create.
 * @return 0 on success, -1 if an error occurred.
 */
int utils_make_dirs(const char* path);

#endif /* UTILS_H */
