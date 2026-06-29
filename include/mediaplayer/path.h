#pragma once

#include <stdbool.h>

#define PATH_MAX_LEN 4096

/* Initialize path module. Call once at plugin startup with the server's working directory. */
void path_init(const char *work_dir);

/* Read-only access to base directories */
const char *path_data(void);
const char *path_nbs(void);
const char *path_video(void);

/* Join two path components with '/'. Returns pointer to internal static buffer. */
const char *path_join(const char *base, const char *name);

/*
 * Strip file extension. Writes result into dst (up to dst_size).
 * "song.nbs" -> "song", "my.song.v2.nbs" -> "my.song.v2"
 */
void path_stem(const char *filename, char *dst, size_t dst_size);

/* Platform-agnostic mkdir (no-op if already exists) */
void path_mkdir(const char *path);

/* Check if a file exists and is readable */
bool path_file_exists(const char *path);
