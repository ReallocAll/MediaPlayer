#pragma once
#include <littlehooker/littlehooker.h>
#include <math.h>
#include "cpp_string.h"
#include "logger.h"
#include "mc/network.h"
#include "mc/actor.h"
#include "mc/player.h"
#include "mc/level.h"

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
    struct music_queue_node *next;
};

struct note_queue_node *generate_note_queue(FILE *fp);
void send_music_sound_packet(void);
bool music_queue_add_player(long long xuid, FILE *fp);
void music_queue_delete_player(long long xuid);
void free_note_queue(struct note_queue_node *head);
void free_music_queue(void);
