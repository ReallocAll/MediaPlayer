#pragma once
#include <littlehooker/littlehooker.h>
#include <math.h>
#include "cpp_string.h"
#include "logger.h"
#include "file_utils.h"
#include "video_player.h"
#include "mc/network.h"
#include "mc/actor.h"
#include "mc/player.h"
#include "mc/level.h"

#define NUM_NOTES 16

static const char *BUILTIN_INSTRUMENT[NUM_NOTES] = {
    "note.harp",
    "note.bassattack",
    "note.bd",
    "note.snare",
    "note.hat",
    "note.guitar",
    "note.flute",
    "note.bell",
    "note.chime",
    "note.xylobone",
    "note.iron_xylophone",
    "note.cow_bell",
    "note.didgeridoo",
    "note.bit",
    "note.banjo",
    "note.pling"
};

enum music_bar_type {
    MUSIC_BAR_TYPE_NOT_DISPLAY,
    MUSIC_BAR_TYPE_BOSS_BAR,
    MUSIC_BAR_TYPE_ACTION_BAR_JUKEBOX_POPUP,
    MUSIC_BAR_TYPE_ACTION_BAR_POPUP,
    MUSIC_BAR_TYPE_ACTION_BAR_TIP
};

struct note_queue_node {
    int time;
    int instrument;
    float volume;
    float pitch;
    struct note_queue_node *next;
};

struct music_queue_node {
    long long xuid;
    struct note_queue_node *note_queue_node;
    struct note_queue_node *note_queue_node_start;
    time_t start_time;
    time_t total_time;
    int loop;
    enum music_bar_type music_bar_type;
    char song_name[256];
    struct music_queue_node *next;
};

struct note_queue_node *generate_note_queue(FILE *fp, time_t *total_time);
void send_music_sound_packet(void);
bool music_queue_add_player(long long xuid, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type);
void music_queue_delete_player(long long xuid);
void free_note_queue(struct note_queue_node *head);
void free_music_queue(void);
struct music_queue_node *music_queue_get_player(long long player_xuid);
bool play_with_video(long long player_xuid, const char *filename, int loop);
void set_music_bar(struct player *player, struct music_queue_node *node);
