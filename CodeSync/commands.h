//
// Created by Harikeshav R on 1/18/25.
//

#ifndef COMMANDS_H
#define COMMANDS_H


#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

struct cmd_struct
{
    const char* cmd;

    int (* fn)(int, const char**);
};


int cmd_add(int argc, const char* argv[]);

int cmd_cat_file(int argc, const char* argv[]);

int cmd_check_ignore(int argc, const char* argv[]);

int cmd_checkout(int argc, const char* argv[]);

int cmd_commit(int argc, const char* argv[]);

int cmd_hash_object(int argc, const char* argv[]);


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
int cmd_init(int argc, const char* argv[]);

int cmd_log(int argc, const char* argv[]);

int cmd_ls_files(int argc, const char* argv[]);

int cmd_ls_tree(int argc, const char* argv[]);

int cmd_rev_parse(int argc, const char* argv[]);

int cmd_rm(int argc, const char* argv[]);

int cmd_show_ref(int argc, const char* argv[]);

int cmd_status(int argc, const char* argv[]);

int cmd_tag(int argc, const char* argv[]);

#endif //COMMANDS_H
