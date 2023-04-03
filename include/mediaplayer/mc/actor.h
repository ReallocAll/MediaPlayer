#pragma once
#include <littlehooker/littlehooker.h>
#include <mediaplayer/cpp_string.h>
#include "position.h"

#define PLAYER_XUID_STR_LEN 17

struct actor;
struct player;

struct vec3 *actor_get_pos(struct actor *actor);
float calc_attack_damage(struct actor *attacker, struct actor *casualty);
const char *get_name_tag(struct actor *actor);
struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt);
const char *get_player_xuid(struct player *player);
unsigned get_entity_type_id(struct actor *actor);
bool is_player(void *ptr);
bool is_player_init(struct player *player);
