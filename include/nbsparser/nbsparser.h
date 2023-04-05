#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct nbs_header {
    unsigned char version;
    unsigned char default_instruments;
    unsigned short song_length;
    unsigned short song_layers;
    const char *song_name;
    const char *song_author;
    const char *original_author;
    const char *description;
    float tempo;
    bool auto_save;
    unsigned char auto_save_duration;
    unsigned char time_signature;
    unsigned int minutes_spent;
    unsigned int left_clicks;
    unsigned int right_clicks;
    unsigned int blocks_added;
    unsigned int blocks_removed;
    const char *song_origin;
    bool loop;
    unsigned char max_loop_count;
    unsigned short loop_start;
};

struct nbs_notes {
    unsigned short tick;
    unsigned short layer;
    unsigned char instrument;
    unsigned char key;
    unsigned char velocity;
    unsigned char panning;
    short pitch;
    struct nbs_notes *next;
};

struct nbs_layers {
    int id;
    const char *name;
    bool lock;
    unsigned char volume;
    short panning;
    struct nbs_layers *next;
};

struct nbs_instruments {
    int id;
    const char *name;
    const char *sound_file;
    unsigned char pitch;
    bool press_key;
    struct nbs_instruments *next;
};

#ifdef __cplusplus
extern "C" {
#endif

const char *nbs_read_string(FILE *fp);

// Return a nbs_header
// After use, please use nbs_free_header() to free it
struct nbs_header *nbs_parse_header(FILE *fp);

// Return a linked list containing nbs_notes structures
// After use, please use nbs_free_notes() to free it
struct nbs_notes *nbs_parse_notes(FILE *fp, unsigned char version);

// Return a linked list containing nbs_layers structures
// After use, please use nbs_free_layers() to free it
struct nbs_layers *nbs_parse_layers(FILE *fp, unsigned short layers_count, unsigned char version);

// Return a linked list containing nbs_instruments structures
// After use, please use nbs_free_instruments() to free it
struct nbs_instruments *nbs_parse_instruments(FILE *fp, unsigned short layers_count, unsigned char version);

void nbs_free_header(struct nbs_header *header);
void nbs_free_notes(struct nbs_notes *head);
void nbs_free_layers(struct nbs_layers *head);
void nbs_free_instruments(struct nbs_instruments *head);

#ifdef __cplusplus
}
#endif
