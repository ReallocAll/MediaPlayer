#pragma once
#include <stdio.h>
#include <time.h>
#include "mc/player.h"

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

// ---- Data structures ----

// A single note, stored in a flat array (no linked list overhead).
struct note {
    long long time;
    int     instrument;
    float   volume;
    float   pitch;
};

// A parsed song cache entry, shared by all players playing the same song.
struct song_cache_entry {
    char           song_name[256];
    struct note   *notes;        // stb_ds array of struct note
    int64_t        duration_ms;  // total duration in milliseconds
};

// One track in a player's playlist.
struct music_queue_entry {
    int                      song_index; // index into g_music_ctx.song_cache
    size_t                   cursor;   // index into song->notes
    int64_t                  start_time; // uv_hrtime() nanosecond timestamp
    int                      loop;
    enum music_bar_type      bar_type;
};

// Per-player music state.
struct player_music {
    struct player              *player;
    char                       *player_xuid;   // strdup'd XUID
    struct music_queue_entry   *playlist;      // stb_ds array
    size_t                      current_track;
    bool                        paused;
};

// Global music player context.
struct music_player_ctx {
    struct song_cache_entry  *song_cache;       // stb_ds array
    struct player_music      *online_players;   // stb_ds array
    struct player_music      *offline_players;  // stb_ds array
};

extern struct music_player_ctx g_music_ctx;

char music_player_save_to_file(void);
void music_player_load_from_file(void);

long long song_cache_parse(FILE *fp, const char *song_name);
void send_music_sound_packet(void);

long long player_music_find(struct player_music *arr, struct player *in_player);
long long player_music_find_by_xuid(struct player_music *arr, const char *in_xuid);

bool player_music_enqueue(struct player *player, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type);
bool player_music_dequeue(struct player *player, size_t index);
void player_music_stop(struct player *player);

void music_player_query_music_queue(struct player *player);

void music_player_player_offline(struct player *in_player);
void music_player_player_online(struct player *in_player);

void set_music_bar_entry(struct player *player, struct music_queue_entry *entry);
