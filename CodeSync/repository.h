//
// Created by Harikeshav R on 1/18/25.
//

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <libconfig.h>

/**
 * Structure representing a repository.
 * It contains paths to the worktree, the .codesync directory, and the repository's configuration.
 */
typedef struct Repository
{
    char* worktree; // Path to the working directory of the repository.
    char* codesync_directory; // Path to the .codesync directory.
    config_t* config; // Pointer to the configuration object.
} Repository;


/**
 * Initializes a repository object with the necessary paths and configuration.
 * It checks if the codesync directory exists and loads the repository configuration.
 *
 * @param repository The repository object to be initialized.
 * @param path The base path to the repository.
 * @param force Flag indicating whether initialization should proceed even if some conditions fail.
 */
void repository_init(Repository* repository, char* path, bool force);


/**
 * Creates a new repository at the specified path. It initializes the repository structure and
 * writes the necessary default files (e.g., HEAD, description, config).
 *
 * @param path The path to create the repository at.
 * @return A pointer to the newly created repository, or NULL if an error occurs.
 */
Repository* repository_create(char* path);


/**
 * Writes the default configuration for the repository to the specified file.
 * This includes the "core" section with settings such as `repository_format_version`, `filemode`, and `bare`.
 *
 * @param repository The repository object that holds the configuration.
 * @param config_file The file where the configuration will be written.
 */
void repository_write_default_config(const Repository* repository, FILE* config_file);

#endif //REPOSITORY_H
