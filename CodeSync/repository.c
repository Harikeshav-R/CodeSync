//
// Created by Harikeshav R on 1/18/25.
//

#include "repository.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"


/**
 * Initializes a repository by setting up the necessary paths and loading the configuration.
 *
 * @param repository The repository object to be initialized.
 * @param path The base path of the repository.
 * @param force Flag indicating whether to force initialization even if some conditions fail.
 */
void repository_init(Repository* repository, char* path, const bool force)
{
    // Get the path to the .codesync directory by appending it to the base path
    char* codesync_directory = utils_join_paths(path, ".codesync");
    char* config_file_path = nullptr;

    // Check if the directory could be resolved
    if (!codesync_directory)
    {
        // Print error if directory resolution fails
        fprintf(stderr, "CodeSync directory not found!\n");
        return;
    }

    // Initialize the string fields in repository
    repository->worktree = path; // Set the worktree path
    repository->codesync_directory = codesync_directory; // Set the codesync directory path

    // Allocate memory for the repository's config object
    repository->config = malloc(sizeof(config_t));
    if (!repository->config)
    {
        // If memory allocation fails, print error and free resources
        fprintf(stderr, "Could not allocate memory for repository->config\n");
        free(codesync_directory);
        return;
    }
    config_init(repository->config); // Initialize the config object

    // Check if the codesync directory exists (unless force flag is set)
    if (!(force || utils_directory_exists(repository->codesync_directory)))
    {
        // If directory doesn't exist and force is not set, report error
        fprintf(stderr, "Not a CodeSync Repository!\n");
        goto cleanup; // Go to cleanup
    }

    // Resolve the path to the config file
    config_file_path = utils_repo_file(repository, false, 1, "config");

    // Check if the config file exists and attempt to read it
    if (config_file_path && utils_path_exists(config_file_path))
    {
        if (!config_read_file(repository->config, config_file_path))
        {
            // If reading the config file fails, report error
            fprintf(stderr, "Error reading config file: %s\n", config_error_text(repository->config));
            goto cleanup; // Go to cleanup
        }
    }
    else if (!force)
    {
        // If the config file is missing and force is not set, report error
        fprintf(stderr, "Configuration file missing!\n");
        goto cleanup; // Go to cleanup
    }

    // If force is not set, check for repository version compatibility
    if (!force)
    {
        int version;
        if (config_lookup_int(repository->config, "core.repository_format_version", &version))
        {
            if (version != 0)
            {
                // If the version is not supported, report error
                fprintf(stderr, "Unsupported repository_format_version: %d!\n", version);
                goto cleanup; // Go to cleanup
            }
        }
    }

    // All operations are successful, no need for cleanup
    return;

cleanup:
    // Free resources in case of an error
    free(config_file_path);
    free(codesync_directory);
    config_destroy(repository->config);
    free(repository->config);
}


/**
 * Creates a new repository at the specified path and initializes it.
 *
 * @param path The path to the new repository.
 * @return A pointer to the newly created repository, or NULL if creation fails.
 */
Repository* repository_create(char* path)
{
    // Allocate memory for the repository object
    Repository* repository = malloc(sizeof(Repository));
    repository_init(repository, path, true); // Initialize the repository

    // Check if the worktree already exists
    if (utils_path_exists(repository->worktree))
    {
        // Ensure the worktree is a directory
        if (!(utils_directory_exists(repository->worktree)))
        {
            fprintf(stderr, "%s is not a directory!\n", path);
            free(repository);
            return nullptr; // Return NULL if not a directory
        }

        // Check if the codesync directory exists and is empty
        if (!utils_path_exists(repository->codesync_directory) && !utils_is_directory_empty(
                repository->codesync_directory))
        {
            fprintf(stderr, "%s is not empty!\n", path);
            free(repository);
            return nullptr; // Return NULL if not empty
        }
    }

    else
    {
        // If the worktree doesn't exist, try creating the directory
        if (utils_make_dirs(repository->worktree) != 0)
        {
            fprintf(stderr, "Could not make repository directory!\n");
            free(repository);
            return nullptr; // Return NULL if directory creation fails
        }
    }

    // Assert that required directories for the repository are created
    assert(utils_repo_dir(repository, true, 1, "branches") != nullptr);
    assert(utils_repo_dir(repository, true, 1, "objects") != nullptr);
    assert(utils_repo_dir(repository, true, 2, "refs", "tags") != nullptr);
    assert(utils_repo_dir(repository, true, 2, "refs", "heads") != nullptr);

    // Write the description file with default content
    FILE* description_file = fopen(utils_repo_file(repository, false, 1, "description"), "w");
    if (!description_file)
    {
        fprintf(stderr, "Could not open description file for writing!\n");
        free(repository);
        return nullptr; // Return NULL if description file can't be opened
    }

    fprintf(description_file, "Unnamed repository; edit this file 'description' to name the repository.\n");
    fclose(description_file); // Close the description file

    // Write the HEAD file with the initial reference to the master branch
    FILE* head_file = fopen(utils_repo_file(repository, false, 1, "HEAD"), "w");
    if (!head_file)
    {
        fprintf(stderr, "Could not open HEAD file for writing!\n");
        free(repository);
        return nullptr; // Return NULL if HEAD file can't be opened
    }

    fprintf(head_file, "ref: refs/heads/master\n");
    fclose(head_file); // Close the HEAD file

    // Write the config file with default values
    FILE* config_file = fopen(utils_repo_file(repository, false, 1, "config"), "w");
    if (!config_file)
    {
        fprintf(stderr, "Could not open config file for writing!\n");
        free(repository);
        return nullptr; // Return NULL if config file can't be opened
    }
    repository_write_default_config(repository, config_file); // Write the default config
    fclose(config_file); // Close the config file

    // Return the created repository
    return repository;
}


/**
 * Writes the default configuration for the repository to the specified file.
 * This includes the "core" section with settings such as `repository_format_version`, `filemode`, and `bare`.
 *
 * @param repository The repository object that holds the configuration.
 * @param config_file The file where the configuration will be written.
 */
void repository_write_default_config(const Repository* repository, FILE* config_file)
{
    // Ensure the "core" section exists or create it if it doesn't
    config_setting_t* core = config_lookup(repository->config, "core");
    if (core == NULL)
    {
        // If "core" section does not exist, create it
        core = config_setting_add(config_root_setting(repository->config), "core", CONFIG_TYPE_GROUP);
    }

    // Set key-value pairs under the "core" section
    config_setting_t* repository_format_version =
            config_setting_add(core, "repository_format_version", CONFIG_TYPE_INT);
    config_setting_set_int(repository_format_version, 0);

    config_setting_t* filemode = config_setting_add(core, "filemode", CONFIG_TYPE_BOOL);
    config_setting_set_bool(filemode, false);

    config_setting_t* bare = config_setting_add(core, "bare", CONFIG_TYPE_BOOL);
    config_setting_set_bool(bare, false);

    // Write the configuration to a file
    config_write(repository->config, config_file);
}


/**
 * Finds a CodeSync repository by searching for a ".codesync" directory in the given path
 * or any of its parent directories.
 *
 * @param path The starting directory to search for the repository.
 * @param required If true, the function will raise an error if no repository is found.
 * @return A pointer to the Repository structure or nullptr if no repository is found and `required` is false.
 */
Repository* repo_find(const char* path, const bool required)
{
    char* resolved_path = realpath(path, nullptr); // Dynamically allocate the resolved path
    if (resolved_path == NULL)
    {
        perror("realpath");
        return nullptr;
    }

    // Construct the ".codesync" path
    const size_t codesync_dir_length = strlen(resolved_path) + strlen("/.codesync") + 1;
    char* codesync_dir = malloc(codesync_dir_length);
    if (codesync_dir == NULL)
    {
        perror("malloc");
        free(resolved_path);
        return nullptr;
    }
    snprintf(codesync_dir, codesync_dir_length, "%s/.codesync", resolved_path);

    // Check if the ".codesync" directory exists
    struct stat stat_buf;
    if (stat(codesync_dir, &stat_buf) == 0 && S_ISDIR(stat_buf.st_mode))
    {
        free(codesync_dir);
        Repository* repository = malloc(sizeof(Repository));
        repository_init(repository, resolved_path, false);
        free(resolved_path);
        return repository;
    }
    free(codesync_dir);

    // Move to the parent directory
    char* parent = malloc(strlen(resolved_path) + strlen("/..") + 1);
    if (parent == NULL)
    {
        perror("malloc");
        free(resolved_path);
        return nullptr;
    }
    snprintf(parent, strlen(resolved_path) + strlen("/..") + 1, "%s/..", resolved_path);
    free(resolved_path);

    // Resolve the parent directory path
    char* parent_resolved = realpath(parent, NULL);
    free(parent);
    if (parent_resolved == NULL)
    {
        perror("realpath");
        return nullptr;
    }

    // Check if we are at the root directory
    if (strcmp(parent_resolved, resolved_path) == 0)
    {
        free(parent_resolved);
        if (required)
        {
            fprintf(stderr, "No CodeSync directory found.\n");
            exit(EXIT_FAILURE);
        }
        return nullptr;
    }

    // Recursive call to search in the parent directory
    Repository* repo = repo_find(parent_resolved, required);
    free(parent_resolved);
    return repo;
}
