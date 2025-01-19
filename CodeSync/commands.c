#include "commands.h"

#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"
#include "repository.h"


/**
 * The usage string for the command-line tool.
 *
 * This provides a basic format for how the command-line tool should be used.
 */
static const char* const usages[] = {
    "subcommands [options] [cmd] [args]",
    nullptr,
};


/**
 * Initializes a new repository at the specified path.
 *
 * This function parses command line arguments to obtain the path where the repository
 * should be created. If the path is not provided, an error message is displayed, and
 * the function returns an exit failure code. If the path is provided, the repository
 * is created at that location.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return 0 on success, EXIT_FAILURE if an error occurs (e.g., missing path argument).
 */
int cmd_init(int argc, const char* argv[])
{
    // Declare a pointer to hold the path argument
    char* path = nullptr;

    // Define the options for command-line arguments using argparse
    struct argparse_option options[] = {
        OPT_HELP(), // Option to display help message
        OPT_STRING('p', "path", &path, "The path to create a repository at", nullptr, 0, 0),
        // Option for repository path
        OPT_END(), // Marks the end of options
    };

    // Initialize the argparse structure
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);

    // Parse the command-line arguments
    argc = argparse_parse(&argparse, argc, argv);

    // Check if the path argument was provided, if not, show an error
    if (path == nullptr)
    {
        fprintf(stderr, "Missing required argument\n");
        return EXIT_FAILURE; // Return failure if no path is provided
    }

    if (repository_find(path, true) == nullptr)
    {
        printf("Failed to find repo, creating new!\n");
        // Call the repository creation function with the provided path
        Repository* repository = repository_create(path);
        repository_free(&repository);
    }
    else
    {
        printf("Repository already exists!\n");
    }

    // Return success as the repository was created
    return 0;
}
