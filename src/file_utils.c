#include <mediaplayer/file_utils.h>

char **get_filenames(const char *directory, int *count)
{
    setlocale(LC_ALL, "");
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    char **filenames = NULL;

    dir = opendir(directory);
    if (dir == NULL) {
        CreateDirectoryA(directory, NULL);
        *count = 0;
        return filenames;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG && ent->d_name[0] != '.' && strstr(ent->d_name, ".nbs")) {
            char *filename = malloc(ent->d_namlen + 1);
            strcpy(filename, ent->d_name);

            filenames = realloc(filenames, sizeof(char *) * (i + 1));
            filenames[i] = filename;
            i++;
        }
    }

    closedir(dir);

    *count = i;
    return filenames;
}

void free_filenames(char **filenames, int count)
{
    for (int i = 0; i < count; i++) {
        free(filenames[i]);
    }
    free(filenames);
}
