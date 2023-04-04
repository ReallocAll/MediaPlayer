#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;

struct nbs_header {
    uchar version;
    uchar default_instruments;
    ushort song_length;
    ushort song_layers;
    char *song_name;
    char *song_author;
    char *original_author;
    char *description;
    float tempo;
    uchar auto_save;
    uchar auto_save_duration;
    uchar time_signature;
    uint minutes_spent;
    uint left_clicks;
    uint right_clicks;
    uint blocks_added;
    uint blocks_removed;
    char *song_origin;
    uchar loop;
    uchar max_loop_count;
    ushort loop_start;
};

struct nbs_notes {
    ushort tick;
    ushort layer;
    uchar instrument;
    uchar key;
    uchar velocity;
    uchar panning;
    short pitch;
    struct nbs_notes *next;
};

struct nbs_layers {
    int id;
    char *name;
    uchar lock;
    uchar volume;
    short panning;
    struct nbs_layers *next;
};

struct nbs_instruments {
    int id;
    char *name;
    char *sound_file;
    uchar pitch;
    uchar press_key;
    struct nbs_instruments *next;
};

char *nbs_read_string(FILE *fp);

struct nbs_header *nbs_parse_header(FILE *fp);
struct nbs_notes *nbs_parse_notes(FILE *fp, uchar version);
struct nbs_layers *nbs_parse_layers(FILE *fp, ushort layers_count, uchar version);
struct nbs_instruments *nbs_parse_instruments(FILE *fp, ushort layers_count, uchar version);

void nbs_free_notes(struct nbs_notes *head);
void nbs_free_layers(struct nbs_layers *head);
void nbs_free_instruments(struct nbs_instruments *head);
