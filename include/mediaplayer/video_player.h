#pragma once

#include <libuv/uv.h>

#include <time.h>
#include <stdio.h>
#include <limits.h>

#include "logger.h"
#include "process_png.h"
#include "file_utils.h"
#include "music_player.h"
#include "window_player.h"

#include "mc/actor.h"
#include "mc/player.h"
#include "mc/level.h"

#ifndef __linux__
typedef uint64_t __uint64_t;
#endif

#define UV_HRT_PER_MS ((__uint64_t) 1000000)

static struct block_pos start_pos  = {INT_MAX, INT_MIN, INT_MAX};
static struct block_pos end_pos = {INT_MIN, INT_MAX, INT_MIN};

enum direction {
    DIRECTION_NEG_Y,
    DIRECTION_POS_Y,
    DIRECTION_NEG_Z,
    DIRECTION_POS_Z,
    DIRECTION_NEG_X,
    DIRECTION_POS_X,
};

struct video_queue {
    struct player *player;
    time_t start_time;
    char video_path[4096];
    int total_frames;
    int current_frame;
    int loop;
    unsigned char *image;
    struct spng_ihdr ihdr;
    bool deleted;
    HWND hwnd;
};

struct screen_pos {
    int x, y;
};

bool video_queue_add_player(struct player *player, char *video_path, int loop);
void video_queue_delete_player(struct player *player);
void play_video(struct video_queue *video_queue_node, struct map_item_saved_data *map_data, struct screen_pos *screen_pos);
void free_video_queue(void);
struct video_queue *video_queue_get_player(struct player *player);
