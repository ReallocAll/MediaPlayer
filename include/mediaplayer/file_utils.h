#pragma once
#include <sys/stat.h>
#include <sys/types.h>

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __linux__
#include <dirent.h>
#else
#include <dirent/dirent.h>
#endif

extern char work_path[];
extern char data_path[];
extern char data_path_nbs[];
extern char data_path_video[];

void make_directory(const char *directory);
char **get_filenames(const char *directory, int *count);
char **get_foldernames(const char *directory, int *count);
void free_filenames(char **filenames, int count);
void free_foldernames(char **filenames, int count);
bool is_file_exist(const char *path);
