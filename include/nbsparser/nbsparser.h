#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// A single note from the NBS file.
struct nbs_note {
    unsigned short tick;
    unsigned short layer;
    unsigned char instrument;
    unsigned char key;
    unsigned char velocity;
    unsigned char panning;
    short          pitch;
};

// A single layer from the NBS file.
struct nbs_layer {
    int          id;
    char        *name;
    bool         lock;
    unsigned char volume;
    short        panning;
};

// A single custom instrument from the NBS file.
struct nbs_instrument {
    int          id;
    char        *name;
    char        *sound_file;
    unsigned char pitch;
    bool         press_key;
};

// Complete parsed NBS song, with all data in flat arrays.
struct nbs_song {
    // Header fields
    unsigned char version;
    unsigned char default_instruments;
    unsigned short song_length;
    unsigned short song_layers;
    char          *song_name;
    char          *song_author;
    char          *original_author;
    char          *description;
    float          tempo;
    bool           auto_save;
    unsigned char  auto_save_duration;
    unsigned char  time_signature;
    unsigned int   minutes_spent;
    unsigned int   left_clicks;
    unsigned int   right_clicks;
    unsigned int   blocks_added;
    unsigned int   blocks_removed;
    char          *song_origin;
    bool           loop;
    unsigned char  max_loop_count;
    unsigned short loop_start;

    // Data arrays (stb_ds)
    struct nbs_note       *notes;
    struct nbs_layer      *layers;
    struct nbs_instrument *instruments;
};

#ifdef __cplusplus
extern "C" {
#endif

// Parse an entire NBS file into a single nbs_song struct.
// Returns nullptr on failure.
// After use, call nbs_free() to release all memory.
struct nbs_song *nbs_parse(FILE *fp);

// Free all memory owned by an nbs_song.
void nbs_free(struct nbs_song *song);

#ifdef __cplusplus
}
#endif
