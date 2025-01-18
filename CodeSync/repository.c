//
// Created by Harikeshav R on 1/18/25.
//


#include <stdlib.h>

#include "repository.h"
#include "utils.h"


void repository_init(Repository* repository, char* path, const bool force)
{
    // Get the path to the .codesync directory by appending it to the base path
    char* codesync_directory = utils_join_paths(path, ".codesync");
    char* config_file_path = nullptr;

    // Check if the directory could be resolved
    if (!codesync_directory)
    {
        fprintf(stderr, "Codesync directory not found!\n");
        return;
    }

    // Initialize the string fields in repository
    repository->worktree = path;
    repository->codesync_directory = codesync_directory;

    // Allocate memory and initialize the config object
    repository->config = malloc(sizeof(config_t));
    if (!repository->config)
    {
        fprintf(stderr, "Could not allocate memory for repository->config\n");
        free(codesync_directory);
        return;
    }
    config_init(repository->config);

    // Check if the codesync directory exists
    if (!(force || utils_directory_exists(repository->codesync_directory)))
    {
        fprintf(stderr, "Not a CodeSync Repository!\n");
        goto cleanup;
    }

    // Resolve the path to the config file
    config_file_path = utils_repo_file(repository, "config", false);

    // Check if the config file path exists
    if (config_file_path && utils_path_exists(config_file_path))
    {
        if (!config_read_file(repository->config, config_file_path))
        {
            fprintf(stderr, "Error reading config file: %s\n", config_error_text(repository->config));
            goto cleanup;
        }
    }
    else if (!force)
    {
        fprintf(stderr, "Configuration file missing!\n");
        goto cleanup;
    }

    // Handle repository version check if not forced
    if (!force)
    {
        int version;
        if (config_lookup_int(repository->config, "core.repositoryformatversion", &version))
        {
            if (version != 0)
            {
                fprintf(stderr, "Unsupported repositoryformatversion: %d!\n", version);
            }
        }
    }

    // All allocations are successful, return without cleanup needed here
    return;

cleanup:
    free(config_file_path);
    free(codesync_directory);
    config_destroy(repository->config);
    free(repository->config);
}
