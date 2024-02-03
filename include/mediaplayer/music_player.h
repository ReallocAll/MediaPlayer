#pragma once
#include <libcutils/libcutils.h>
#include <nbsparser/nbsparser.h>
#include <math.h>
#include <time.h>
#include <libcutils/libcutils.h>
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
    // time_t time; //what the fuck!!!!!!!!!!!!!!!!!!!!!!!!!!!
    long long time;
    int instrument;
    float volume;
    float pitch;
    struct note_queue_node *next;
};

struct music_queue_node {
    struct player *player;
    struct note_queue_node *note_queue_node;
    struct note_queue_node *note_queue_node_start;
    time_t start_time;
    time_t total_time;
    int loop;
    enum music_bar_type music_bar_type;
    char song_name[256];
    struct music_queue_node *prev;
    struct music_queue_node *next;
};

struct player_music_info {
    struct player *player;
    char *player_xuid;
    __int64_t music_num;
    char paused;
    struct music_queue_node *music_queue_node;
};

struct note_queue_node *generate_note_queue(FILE *fp, time_t *total_time);
void send_music_sound_packet(void);

long long find_player_in_array(struct player_music_info *in_array, unsigned long long in_array_size, struct player *in_player);

bool music_queue_add(struct player *player, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type);
bool music_queue_del(struct player *player, unsigned long long in_pos);
void music_queue_del_all(struct player *player);

void music_player_query_music_queue(struct player *player);

void music_player_player_offline(struct player *in_player);
void music_player_player_online(struct player *in_player);

void free_note_queue(struct note_queue_node *head);
void free_music_queue(void);
struct music_queue_node *music_queue_get_player(struct player *player);
bool play_with_video(struct player *player, const char *filename, int loop);
void set_music_bar(struct player *player, struct music_queue_node *node);
