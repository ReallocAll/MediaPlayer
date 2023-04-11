#pragma once
#include <littlehooker/littlehooker.h>
#include "logger.h"
#include "file_utils.h"
#include "mc/actor.h"
#include "mc/player.h"
#include "mc/level.h"

struct video_queue {
    long long xuid;
    time_t start_time;
    char video_path[260];
    int total_frames;
    int current_frame;
    struct video_queue *next;
};

bool video_queue_add_player(long long xuid, char *video_path);
void video_queue_delete_player(long long xuid);
void play_video(struct video_queue *video_queue_node, struct map_item_saved_data *map_data);
void free_video_queue(void);
struct video_queue *video_queue_get_player(long long player_xuid);
