#pragma once
#include <littlehooker/littlehooker.h>
#include <mediaplayer/cpp_string.h>
#include "position.h"

struct actor;

struct vec3 *actor_get_pos(struct actor *actor);
float calc_attack_damage(struct actor *attacker, struct actor *casualty);
const char *get_name_tag(struct actor *actor);
unsigned get_entity_type_id(struct actor *actor);
