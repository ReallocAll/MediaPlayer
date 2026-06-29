#pragma once

#include <stdbool.h>

/*
 * List entries in a directory.
 * If dirs_only is true, returns directory names; otherwise returns file names.
 * Entries starting with '.' are skipped.
 * Caller must free the result with free_dirlist().
 */
char **list_directory(const char *directory, int *count, bool dirs_only);

/* Free a list returned by list_directory() */
void free_dirlist(char **list, int count);
