#pragma warning(disable:4996)
#include <nbsparser/nbsparser.h>

#define ushort unsigned short
#define uint unsigned int
#define uchar unsigned char

const char *nbs_read_string(FILE *fp)
{
    uint str_len;
    char buffer[256];
    fread(&str_len, sizeof(uint), 1, fp);
    fread(buffer, sizeof(char), str_len, fp);
    buffer[str_len] = '\0';
    return strdup(buffer);
}

struct nbs_header *nbs_parse_header(FILE *fp)
{
    struct nbs_header *header = (struct nbs_header *) malloc(sizeof(struct nbs_header));
    if (!header)
        return (struct nbs_header *)NULL;

    ushort song_length = 0;
    ushort tempo = 0;

    fread(&song_length, sizeof(ushort), 1, fp);
    if (song_length == 0)
        fread(&header->version, sizeof(uchar), 1, fp);
    else
        header->version = 0;

    if (header->version > 0)
        fread(&header->default_instruments, sizeof(uchar), 1, fp);
    else
        header->default_instruments = 10;
    
    if (header->version >= 3)
        fread(&header->song_length, sizeof(ushort), 1, fp);
    else
        header->song_length = song_length;

    fread(&header->song_layers, sizeof(ushort), 1, fp);
    header->song_name = nbs_read_string(fp);
    header->song_author = nbs_read_string(fp);
    header->original_author = nbs_read_string(fp);
    header->description = nbs_read_string(fp);

    fread(&tempo, sizeof(ushort), 1, fp);
    header->tempo = (float)tempo / 100.0f;

    fread(&header->auto_save, sizeof(uchar), 1, fp);
    fread(&header->auto_save_duration, sizeof(uchar), 1, fp);
    fread(&header->time_signature, sizeof(uchar), 1, fp);
    fread(&header->minutes_spent, sizeof(uint), 1, fp);
    fread(&header->left_clicks, sizeof(uint), 1, fp);
    fread(&header->right_clicks, sizeof(uint), 1, fp);
    fread(&header->blocks_added, sizeof(uint), 1, fp);
    fread(&header->blocks_removed, sizeof(uint), 1, fp);
    header->song_origin = nbs_read_string(fp);

    if (header->version >= 4)
        fread(&header->loop, sizeof(uchar), 1, fp);
    else
        header->loop = false;

    if (header->version >= 4)
        fread(&header->max_loop_count, sizeof(uchar), 1, fp);
    else
        header->max_loop_count = 0;

    if (header->version >= 4)
        fread(&header->loop_start, sizeof(ushort), 1, fp);
    else
        header->loop_start = 0;

    return header;
}

struct nbs_notes *nbs_parse_notes(FILE *fp, unsigned char version)
{
    struct nbs_notes *head = NULL;
    struct nbs_notes *tail = NULL;

    int current_tick = -1;
    int current_layer = -1;
    uchar instrument;
    uchar key;
    uchar velocity;
    uchar panning;
    short pitch;

    ushort jump;
    while (true) {
        fread(&jump, sizeof(ushort), 1, fp);
        if (!jump) {
            current_tick = -1;
            break;
        }
        current_tick += jump;

        while (true) {
            fread(&jump, sizeof(ushort), 1, fp);
            if (!jump) {
                current_layer = -1;
                break;
            }
            current_layer += jump;

            fread(&instrument, sizeof(uchar), 1, fp);
            fread(&key, sizeof(uchar), 1, fp);

            if (version >= 4)
                fread(&velocity, sizeof(uchar), 1, fp);
            else
                velocity = 100;

            if (version >= 4) {
                fread(&panning, sizeof(uchar), 1, fp);
                panning -= 100;
            } else {
                panning = 0;
            }

            if (version >= 4)
                fread(&pitch, sizeof(short), 1, fp);
            else
                pitch = 0;

            struct nbs_notes *new_note = (struct nbs_notes *) malloc(sizeof(struct nbs_notes));
            if (!new_note)
                return (struct nbs_notes *)NULL;

            new_note->tick = current_tick;
            new_note->layer = current_layer;
            new_note->instrument = instrument;
            new_note->key = key;
            new_note->velocity = velocity;
            new_note->panning = panning;
            new_note->pitch = pitch;
            new_note->next = NULL;

            if (head == NULL) {
                head = new_note;
                tail = new_note;
            } else {
                tail->next = new_note;
                tail = new_note;
            }
        }
    }
    return head;
}

struct nbs_layers *nbs_parse_layers(FILE *fp, unsigned short layers_count, unsigned char version)
{
    struct nbs_layers *head = NULL;
    struct nbs_layers *tail = NULL;

    const char *name;
    uchar lock;
    uchar volume;
    uchar panning;

    for (int i = 0; i < layers_count; i++) {
        name = nbs_read_string(fp);
        if (version >= 4)
            fread(&lock, sizeof(uchar), 1, fp);
        else
            lock = 0;

        fread(&volume, sizeof(uchar), 1, fp);
        
        if (version >= 2) {
            fread(&panning, sizeof(uchar), 1, fp);
            panning -= 100;
        } else {
            panning = 0;
        }

        struct nbs_layers *new_layer = (struct nbs_layers *) malloc(sizeof(struct nbs_layers));
        if (!new_layer)
            return (struct nbs_layers *)NULL;

        new_layer->id = i;
        new_layer->name = name;
        new_layer->lock = (bool)lock;
        new_layer->volume = volume;
        new_layer->panning = (short)panning;
        new_layer->next = NULL;

        if (head == NULL) {
            head = new_layer;
            tail = new_layer;
        } else {
            tail->next = new_layer;
            tail = new_layer;
        }
    }
    return head;
}

struct nbs_instruments *nbs_parse_instruments(FILE *fp, unsigned short layers_count, unsigned char version)
{
    struct nbs_instruments *head = NULL;
    struct nbs_instruments *tail = NULL;

    const char *name;
    const char *sound_file;
    uchar pitch;
    uchar press_key;

    uchar instrument_count;
    fread(&instrument_count, sizeof(uchar), 1, fp);

    for (int i = 0; i < instrument_count; i++) {
        name = nbs_read_string(fp);
        sound_file = nbs_read_string(fp);
        fread(&pitch, sizeof(uchar), 1, fp);
        fread(&press_key, sizeof(uchar), 1, fp);

        struct nbs_instruments *new_instrument = (struct nbs_instruments *) malloc(sizeof(struct nbs_instruments));
        if (!new_instrument)
            return (struct nbs_instruments *)NULL;

        new_instrument->id = i;
        new_instrument->name = name;
        new_instrument->sound_file = sound_file;
        new_instrument->pitch = pitch;
        new_instrument->press_key = (bool)press_key;
        new_instrument->next = NULL;

        if (head == NULL) {
            head = new_instrument;
            tail = new_instrument;
        } else {
            tail->next = new_instrument;
            tail = new_instrument;
        }
    }
    return head;
}

void nbs_free_header(struct nbs_header *header)
{
    free(header);
    header = NULL;
}

void nbs_free_notes(struct nbs_notes *head)
{
    struct nbs_notes *current = head;
    while (current != NULL) {
        struct nbs_notes *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

void nbs_free_layers(struct nbs_layers *head)
{
    struct nbs_layers *current = head;
    while (current != NULL) {
        struct nbs_layers *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}

void nbs_free_instruments(struct nbs_instruments *head)
{
    struct nbs_instruments *current = head;
    while (current != NULL) {
        struct nbs_instruments *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}
