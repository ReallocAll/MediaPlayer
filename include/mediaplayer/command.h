#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "music_player.h"
#include "video_player.h"
#include "file_utils.h"
#include "mc/actor.h"
#include "mc/player.h"
#include "mc/network.h"
#include "mc/level.h"

bool process_cmd(struct player *player, const char *cmd);
