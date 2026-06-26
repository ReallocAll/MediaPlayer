#pragma once
#include <stdbool.h>

struct player;

bool process_cmd(struct player *player, const char *cmd);
