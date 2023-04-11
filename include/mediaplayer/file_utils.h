#pragma once
#include <windows.h>
#include <locale.h>
#include <dirent/dirent.h>

extern char bds_path[];
extern char data_path[];
extern char data_path_nbs[];
extern char data_path_video[];

char **get_filenames(const char *directory, int *count);
char **get_foldernames(const char *directory, int *count);
void free_filenames(char **filenames, int count);
void free_foldernames(char **filenames, int count);
