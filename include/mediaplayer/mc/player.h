#pragma once
#pragma warning(disable: 4996)
#include <libcutils/libcutils.h>
#include <mediaplayer/mc/symbols.h>
#include "structs.h"
#include "network.h"
#include "position.h"

#define PLAYER_XUID_STR_LEN 17

extern struct player *(*g_player_list);
extern int g_player_list_size;

struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt);
const char *get_player_xuid(struct player *player);
bool is_player_init(struct player *player);

// returns the index of the player in the list, or -1 if it does not exist.
int player_list_get(struct player *player);

void player_list_add(struct player *player);

void player_list_delete(struct player *player);
