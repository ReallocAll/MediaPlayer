#pragma once
#include <libcutils/libcutils.h>
#include <mediaplayer/mc/symbols.h>
#include "position.h"

struct actor;

struct vec3 *actor_get_pos(struct actor *actor);
const char *get_name_tag(struct actor *actor);
uintptr_t get_or_create_unique_id(struct actor *actor);
