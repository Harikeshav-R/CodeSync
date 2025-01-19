//
// Created by Harikeshav R on 1/18/25.
//

#include "repository.h"

#include <assert.h>
#include <stdlib.h>

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
        fprintf(stderr, "Codesync directory not found!\n");
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
