#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __linux__
#include <dirent.h>
#else
#include <windows.h>
#endif

#include <mediaplayer/path.h>

static char s_data[PATH_MAX_LEN];
static char s_nbs[PATH_MAX_LEN];
static char s_video[PATH_MAX_LEN];
static char s_join[PATH_MAX_LEN];

void path_init(const char *work_dir)
{
#ifndef __linux__
	setlocale(LC_ALL, ".UTF-8");
#endif
	snprintf(s_data, PATH_MAX_LEN, "%s/plugins/MediaPlayer", work_dir);
	snprintf(s_nbs, PATH_MAX_LEN, "%s/nbs", s_data);
	snprintf(s_video, PATH_MAX_LEN, "%s/video", s_data);

	path_mkdir(s_data);
	path_mkdir(s_nbs);
	path_mkdir(s_video);
}

const char *path_data(void)  { return s_data; }
const char *path_nbs(void)   { return s_nbs; }
const char *path_video(void) { return s_video; }

const char *path_join(const char *base, const char *name)
{
	snprintf(s_join, PATH_MAX_LEN, "%s/%s", base, name);
	return s_join;
}

void path_stem(const char *filename, char *dst, size_t dst_size)
{
	if (!dst || dst_size == 0) return;

	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) {
		strncpy(dst, filename, dst_size - 1);
		dst[dst_size - 1] = '\0';
		return;
	}

	size_t len = (size_t)(dot - filename);
	if (len >= dst_size) len = dst_size - 1;
	memcpy(dst, filename, len);
	dst[len] = '\0';
}

void path_mkdir(const char *path)
{
#ifdef __linux__
	mkdir(path, S_IRWXU | S_IRWXG | S_IROTH);
#else
	CreateDirectoryA(path, NULL);
#endif
}

bool path_file_exists(const char *path)
{
	FILE *fp = fopen(path, "rb");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}
