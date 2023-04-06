#pragma once
#include <windows.h>
#include <dirent/dirent.h>

char **get_filenames(const char *directory, int *count);
void free_filenames(char **filenames, int count);
