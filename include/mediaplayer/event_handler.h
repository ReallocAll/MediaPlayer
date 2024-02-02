#pragma once
#include "plugin.h"
#include "logger.h"
#include "xiziya_r/misc/xr_dynamic_array.h"

void event_on_server_init_logger();
void event_on_player_left(struct player *in_player);