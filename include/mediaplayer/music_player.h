#pragma once
#include <libcutils/libcutils.h>
#include <nbsparser/nbsparser.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
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

struct music_note_info {
    char song_name[256];
    struct note_queue_node *note_queue_ptr;
    long long time;
};

struct note_queue_node {
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

/* ---- Phase 4: new data structures ---- */

/* A single note, stored in a flat array (no linked list overhead). */
typedef struct {
    long long time;
    int     instrument;
    float   volume;
    float   pitch;
} note_t;

/* A parsed song cache entry, shared by all players playing the same song. */
typedef struct {
    char     song_name[256];
    note_t  *notes;       /* stb_ds array of note_t */
    time_t   duration_ms; /* total duration in milliseconds */
} song_cache_entry_t;

/* One track in a player's playlist. */
typedef struct {
    song_cache_entry_t *song;    /* pointer into g_song_cache */
    size_t              cursor;  /* index into song->notes */
    time_t              start_time;
    int                 loop;
    enum music_bar_type bar_type;
} music_queue_entry_t;

/* Per-player music state. */
typedef struct {
    struct player           *player;
    char                    *player_xuid;  /* strdup'd XUID */
    music_queue_entry_t     *playlist;     /* stb_ds array */
    size_t                   current_track;
    bool                     paused;
} player_music_t;

char music_player_save_to_file(void);

song_cache_entry_t *song_cache_parse(FILE *fp, const char *song_name);
void send_music_sound_packet(void);

long long player_music_find(player_music_t *arr, struct player *in_player);
long long player_music_find_by_xuid(player_music_t *arr, const char *in_xuid);

bool player_music_enqueue(struct player *player, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type);
bool player_music_dequeue(struct player *player, size_t index);
void player_music_stop(struct player *player);

void music_player_query_music_queue(struct player *player);

void music_player_player_offline(struct player *in_player);
void music_player_player_online(struct player *in_player);

void set_music_bar_entry(struct player *player, music_queue_entry_t *entry);
