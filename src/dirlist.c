#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <dirent.h>
#else
#include <dirent/dirent.h>
#endif

#include <mediaplayer/dirlist.h>
#include <mediaplayer/path.h>

char **list_directory(const char *directory, int *count, bool dirs_only)
{
	DIR *dir = opendir(directory);
	if (!dir) {
		path_mkdir(directory);
		*count = 0;
		return NULL;
	}

	char **list = NULL;
	int n = 0;
	struct dirent *ent;
	int target_type = dirs_only ? DT_DIR : DT_REG;

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_type != target_type || ent->d_name[0] == '.')
			continue;

		char *name = malloc(strlen(ent->d_name) + 1);
		if (!name) break;
		strcpy(name, ent->d_name);

		char **tmp = realloc(list, sizeof(char *) * (n + 1));
		if (!tmp) {
			free(name);
			break;
		}
		list = tmp;
		list[n++] = name;
	}

	closedir(dir);
	*count = n;
	return list;
}

void free_dirlist(char **list, int count)
{
	if (!list) return;
	for (int i = 0; i < count; i++)
		free(list[i]);
	free(list);
}
