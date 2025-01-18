//
// Created by Harikeshav R on 1/18/25.
//

#ifndef COMMANDS_H
#define COMMANDS_H

int cmd_add(int argc, const char* argv[]);

int cmd_cat_file(int argc, const char* argv[]);

int cmd_check_ignore(int argc, const char* argv[]);

int cmd_checkout(int argc, const char* argv[]);

int cmd_commit(int argc, const char* argv[]);

int cmd_hash_object(int argc, const char* argv[]);

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
