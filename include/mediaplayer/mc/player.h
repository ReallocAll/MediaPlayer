#pragma once
#include <littlehooker/littlehooker.h>
#include <mediaplayer/cpp_string.h>
#include "position.h"

#define PLAYER_XUID_STR_LEN 17

struct player;

struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt);
const char *get_player_xuid(struct player *player);
bool is_player(void *ptr);
bool is_player_init(struct player *player);
