#pragma warning(disable:4996)
#include <nbsparser/nbsparser.h>
#include <stb/stb_ds.h>

#define ushort unsigned short
#define uint   unsigned int
#define uchar  unsigned char

static char *nbs_read_string_raw(FILE *fp)
{
    uint str_len;
    char buffer[256];
    fread(&str_len, sizeof(uint), 1, fp);
    fread(buffer, sizeof(char), str_len, fp);
    buffer[str_len] = '\0';
    return strdup(buffer);
}

static void parse_header(FILE *fp, struct nbs_song *song)
{
    ushort song_length = 0;
    ushort tempo = 0;

    fread(&song_length, sizeof(ushort), 1, fp);
    if (song_length == 0)
        fread(&song->version, sizeof(uchar), 1, fp);
    else
        song->version = 0;

    if (song->version > 0)
        fread(&song->default_instruments, sizeof(uchar), 1, fp);
    else
        song->default_instruments = 10;

    if (song->version >= 3)
        fread(&song->song_length, sizeof(ushort), 1, fp);
    else
        song->song_length = song_length;

    fread(&song->song_layers, sizeof(ushort), 1, fp);
    song->song_name = nbs_read_string_raw(fp);
    song->song_author = nbs_read_string_raw(fp);
    song->original_author = nbs_read_string_raw(fp);
    song->description = nbs_read_string_raw(fp);

    fread(&tempo, sizeof(ushort), 1, fp);
    song->tempo = (float)tempo / 100.0f;

    fread(&song->auto_save, sizeof(uchar), 1, fp);
    fread(&song->auto_save_duration, sizeof(uchar), 1, fp);
    fread(&song->time_signature, sizeof(uchar), 1, fp);
    fread(&song->minutes_spent, sizeof(uint), 1, fp);
    fread(&song->left_clicks, sizeof(uint), 1, fp);
    fread(&song->right_clicks, sizeof(uint), 1, fp);
    fread(&song->blocks_added, sizeof(uint), 1, fp);
    fread(&song->blocks_removed, sizeof(uint), 1, fp);
    song->song_origin = nbs_read_string_raw(fp);

    if (song->version >= 4)
        fread(&song->loop, sizeof(uchar), 1, fp);
    else
        song->loop = false;

    if (song->version >= 4)
        fread(&song->max_loop_count, sizeof(uchar), 1, fp);
    else
        song->max_loop_count = 0;

    if (song->version >= 4)
        fread(&song->loop_start, sizeof(ushort), 1, fp);
    else
        song->loop_start = 0;
}

static void parse_notes(FILE *fp, struct nbs_song *song)
{
    int current_tick = -1;
    int current_layer = -1;
    ushort jump;

    song->notes = nullptr;

    while (true) {
        fread(&jump, sizeof(ushort), 1, fp);
        if (!jump)
            break;
        current_tick += jump;

        while (true) {
            fread(&jump, sizeof(ushort), 1, fp);
            if (!jump)
                break;
            current_layer += jump;

            struct nbs_note note;
            note.tick = (unsigned short)current_tick;
            note.layer = (unsigned short)current_layer;

            fread(&note.instrument, sizeof(uchar), 1, fp);
            fread(&note.key, sizeof(uchar), 1, fp);

            if (song->version >= 4)
                fread(&note.velocity, sizeof(uchar), 1, fp);
            else
                note.velocity = 100;

            if (song->version >= 4) {
                fread(&note.panning, sizeof(uchar), 1, fp);
                note.panning -= 100;
            } else {
                note.panning = 0;
            }

            if (song->version >= 4)
                fread(&note.pitch, sizeof(short), 1, fp);
            else
                note.pitch = 0;

            arrput(song->notes, note);
        }
    }
}

static void parse_layers(FILE *fp, struct nbs_song *song)
{
    song->layers = nullptr;

    for (int i = 0; i < song->song_layers; i++) {
        struct nbs_layer layer;
        layer.id = i;
        layer.name = nbs_read_string_raw(fp);

        uchar lock = 0;
        if (song->version >= 4)
            fread(&lock, sizeof(uchar), 1, fp);
        layer.lock = (bool)lock;

        fread(&layer.volume, sizeof(uchar), 1, fp);

        uchar panning = 0;
        if (song->version >= 2) {
            fread(&panning, sizeof(uchar), 1, fp);
            layer.panning = (short)(panning - 100);
        } else {
            layer.panning = 0;
        }

        arrput(song->layers, layer);
    }
}

static void parse_instruments(FILE *fp, struct nbs_song *song)
{
    song->instruments = nullptr;

    uchar instrument_count;
    fread(&instrument_count, sizeof(uchar), 1, fp);

    for (int i = 0; i < instrument_count; i++) {
        struct nbs_instrument instr;
        instr.id = i;
        instr.name = nbs_read_string_raw(fp);
        instr.sound_file = nbs_read_string_raw(fp);

        fread(&instr.pitch, sizeof(uchar), 1, fp);

        uchar press_key;
        fread(&press_key, sizeof(uchar), 1, fp);
        instr.press_key = (bool)press_key;

        arrput(song->instruments, instr);
    }
}

struct nbs_song *nbs_parse(FILE *fp)
{
    struct nbs_song *song = (struct nbs_song *)calloc(1, sizeof(struct nbs_song));
    if (!song)
        return nullptr;

    parse_header(fp, song);
    parse_notes(fp, song);
    parse_layers(fp, song);
    parse_instruments(fp, song);

    return song;
}

void nbs_free(struct nbs_song *song)
{
    if (!song)
        return;

    free(song->song_name);
    free(song->song_author);
    free(song->original_author);
    free(song->description);
    free(song->song_origin);

    for (int i = 0; i < (int)arrlen(song->layers); i++)
        free(song->layers[i].name);
    for (int i = 0; i < (int)arrlen(song->instruments); i++) {
        free(song->instruments[i].name);
        free(song->instruments[i].sound_file);
    }

    arrfree(song->notes);
    arrfree(song->layers);
    arrfree(song->instruments);
    free(song);
}
