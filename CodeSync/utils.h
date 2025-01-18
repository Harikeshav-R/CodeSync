//
// Created by Harikeshav R on 1/18/25.
//


#ifndef UTILS_H
#define UTILS_H

#include "repository.h"

bool utils_path_exists(const char* path);

bool utils_directory_exists(const char* path);

char* utils_join_paths(const char* base, const char* path);

char* utils_repo_path(const Repository* repo, const char* path, ...);

char* utils_repo_file(const Repository* repo, const char* path, bool mkdir_flag);

char* utils_repo_dir(const Repository* repo, const char* path, bool mkdir_flag);

#endif /* UTILS_H */
