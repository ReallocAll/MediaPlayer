#include <mediaplayer/music_player.h>
#include <mediaplayer/nbs/nbs.h>
#include <mediaplayer/nbs/遠い空へ.h>

extern struct level *g_level;

struct music_queue_node {
    long long xuid;
    int last_played_position;
    time_t start_time;
    struct music_queue_node *next;
};

struct music_queue_node *music_queue_head = NULL;

void music_queue_add_player(long long xuid)
{
    struct music_queue_node *new_node = (struct music_queue_node *) malloc(sizeof(struct music_queue_node));
    if (new_node == NULL) {
        server_logger("Failed to allocate memory for new node.", ERR);
        return;
    }
    new_node->xuid = xuid;
    new_node->last_played_position = 0;
    new_node->start_time = clock();
    new_node->next = NULL;

    if (music_queue_head == NULL) {
        music_queue_head = new_node;
        return;
    }

    struct music_queue_node *curr_node = music_queue_head;
    while (curr_node->next != NULL) {
        curr_node = curr_node->next;
    }
    curr_node->next = new_node;
}

void music_queue_delete_player(long long xuid)
{
    struct music_queue_node *curr_node = music_queue_head;
    struct music_queue_node *prev_node = NULL;

    while (curr_node != NULL) {
        if (curr_node->xuid == xuid) {
            if (prev_node == NULL)
                music_queue_head = curr_node->next;
            else
                prev_node->next = curr_node->next;

            free(curr_node);
            return;
        }
        prev_node = curr_node;
        curr_node = curr_node->next;
    }
    char msg[128];
    sprintf_s(msg, sizeof(msg), "Player with xuid %lld not found", xuid);
    server_logger(msg, ERR);
}

void music_queue_free_queue(void)
{
    struct music_queue_node *curr_node = music_queue_head;
    while (curr_node != NULL) {
        struct music_queue_node *next_node = curr_node->next;
        free(curr_node);
        curr_node = next_node;
    }
    music_queue_head = NULL;
}

void send_music_sound_packet(void)
{
    struct player *player = NULL;
    const char *player_name;
    struct vec3 *player_pos;
    char player_xuid[PLAYER_XUID_STR_LEN];
    long long note_index = 0;
    const char *sound_name;
    float volume;
    float pitch;

    struct music_queue_node *curr_node = music_queue_head;
    while (curr_node != NULL) {
        sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", curr_node->xuid);
        player = get_player_by_xuid(g_level, player_xuid);
        if (!player) {
            struct music_queue_node *to_delete = curr_node;
            curr_node = curr_node->next;
            music_queue_delete_player(to_delete->xuid);
            continue;
        }
        player_pos = actor_get_pos((struct actor *)player);

        for (note_index = curr_node->last_played_position;
            (time_t)NOTE_DATA[note_index][0] + curr_node->start_time < clock()
            && note_index < MAX_NOTE_LEN;
            note_index++) {

            sound_name = BUILTIN_INSTRUMENT[(int)(NOTE_DATA[note_index][1])];
            volume = NOTE_DATA[note_index][2];
            pitch = NOTE_DATA[note_index][3];

            send_play_sound_packet(player, sound_name, player_pos, volume, pitch);
        }

        if (note_index < MAX_NOTE_LEN) {
            curr_node->last_played_position = note_index;
        } else {
            curr_node->last_played_position = 0;
            curr_node->start_time = (time_t)clock();
        }

        curr_node = curr_node->next;
    }
}
