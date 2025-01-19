#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"
#include "commands.h"


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
 * The structure to hold the commands and their corresponding functions.
 *
 * Each command is associated with a function to handle the logic for that command.
 * The array `commands` holds the mapping between the command name and the function to run.
 */
static struct cmd_struct commands[] = {
    // {"add", cmd_add},
    // {"cat-file", cmd_cat_file},
    // {"check-ignore", cmd_check_ignore},
    // {"checkout", cmd_check_ignore},
    // {"commit", cmd_commit},
    // {"hash-object", cmd_hash_object},
    {"init", cmd_init},
    // {"log", cmd_log},
    // {"ls-files", cmd_ls_files},
    // {"ls-tree", cmd_ls_tree},
    // {"rev-parse", cmd_rev_parse},
    // {"rm", cmd_rm},
    // {"show-ref", cmd_show_ref},
    // {"status", cmd_status},
    // {"tag", cmd_tag},
};


/**
 * Main function to process command-line arguments and run the corresponding command.
 *
 * This function parses the command-line arguments using the `argparse` library,
 * finds the matching command, and executes the corresponding function. If no
 * command is provided or an invalid command is passed, it shows the usage message.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return Returns the exit status of the command function, or 0 if no valid command is found.
 */
int main(int argc, const char* argv[])
{
    // Declare an argparse structure and define the available command-line options
    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_HELP(), // Option to show help message
        OPT_END(), // Marks the end of options
    };

    // Initialize the argparse library with the options and usage strings
    argparse_init(&argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);

    // Parse the command-line arguments
    argc = argparse_parse(&argparse, argc, argv);

    // Check if there are any command-line arguments left to process
    if (argc < 1)
    {
        argparse_usage(&argparse); // Display usage message if no command is provided
        return -1; // Return failure if no command is specified
    }

    /* Try to run the command with arguments provided. */
    // Initialize the pointer to the command structure
    const struct cmd_struct* cmd = nullptr;

    // Iterate over the commands array to find the command that matches the input
    for (int i = 0; i < ARRAY_SIZE(commands); i++)
    {
        if (!strcmp(commands[i].cmd, argv[0]))
        {
            cmd = &commands[i]; // Set the command if found
        }
    }

    // If a matching command is found, execute its corresponding function
    if (cmd)
    {
        return cmd->fn(argc, argv); // Execute the command and return its result
    }

    // Return 0 if no matching command is found
    return 0;
}


