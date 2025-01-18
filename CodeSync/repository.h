//
// Created by Harikeshav R on 1/18/25.
//

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <libconfig.h>


typedef struct Repository
{
    char* worktree;
    char* codesync_directory;
    config_t* config;
} Repository;

void repository_init(Repository* repository, char* path, bool force);

#endif //REPOSITORY_H
