#pragma once
#include <littlehooker/littlehooker.h>
#include <mediaplayer/cpp_string.h>
#include "actor.h"

struct level;

struct player *get_player_by_xuid(const char *xuid);

extern struct level *g_level;
