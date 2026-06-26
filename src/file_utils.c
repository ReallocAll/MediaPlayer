#include <mediaplayer/file_utils.h>

char data_path[4096];
char data_path_nbs[4096];
char data_path_video[4096];

void make_directory(const char *directory)
{
	#ifdef __linux__
	mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH);
	#else
	CreateDirectoryA(directory, nullptr);
	#endif
}

char **get_filenames(const char *directory, int *count)
{
	setlocale(LC_ALL, "en_US.UTF-8");
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	char **filenames = nullptr;

	dir = opendir(directory);
	if (dir == nullptr) {
		make_directory(directory);
		*count = 0;
		return filenames;
	}

	while ((ent = readdir(dir)) != nullptr) {
		if (ent->d_type == DT_REG && ent->d_name[0] != '.') {
			char *filename = malloc(strlen(ent->d_name) + 1);
			char **tmp;

			if (!filename)
				break;
			strcpy(filename, ent->d_name);

			tmp = realloc(filenames, sizeof(char *) * (i + 1));
			if (!tmp) {
				free(filename);
				break;
			}
			filenames = tmp;
			filenames[i] = filename;
			i++;
		}
	}

	closedir(dir);

	*count = i;
	return filenames;
}


char **get_foldernames(const char *directory, int *count)
{
	setlocale(LC_ALL, "en_US.UTF-8");
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	char **foldernames = nullptr;

	dir = opendir(directory);
	if (dir == nullptr) {
		make_directory(directory);
		*count = 0;
		return foldernames;
	}

	while ((ent = readdir(dir)) != nullptr) {
		if (ent->d_type == DT_DIR && ent->d_name[0] != '.') {
			char *foldername = malloc(strlen(ent->d_name) + 1);
			char **tmp;

			if (!foldername)
				break;
			strcpy(foldername, ent->d_name);

			tmp = realloc(foldernames, sizeof(char *) * (i + 1));
			if (!tmp) {
				free(foldername);
				break;
			}
			foldernames = tmp;
			foldernames[i] = foldername;
			i++;
		}
	}

	closedir(dir);

	*count = i;
	return foldernames;
}


void free_filenames(char **filenames, int count)
{
	if (!filenames)
		return;
	for (int i = 0; i < count; i++)
		free(filenames[i]);
	free(filenames);
}


bool is_file_exist(const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}
