#pragma once

#include <time.h>

#include <stdint.h>
#include <libuv/uv.h>
#include <libspng/spng.h>

#include "mc/position.h"
#include "mc/player.h"
#include "mc/structs.h"

#define UV_HRT_PER_MS ((int64_t) 1000000)

extern struct block_pos start_pos;
extern struct block_pos end_pos;


struct video_queue {
    struct player *player;
    uint64_t start_time;
    char video_path[4096];
    int total_frames;
    int current_frame;
    int loop;
    unsigned char *image;
    struct spng_ihdr ihdr;
    bool deleted;
    uv_thread_t tid;
};

struct screen_pos {
    int x, y;
};

bool video_queue_add_player(struct player *player, char *video_path, int loop);
void video_queue_delete_player(struct player *player);
void play_video(struct video_queue *video_queue_node, struct map_item_saved_data *map_data, struct screen_pos *screen_pos);
struct video_queue *video_queue_get_player(struct player *player);
