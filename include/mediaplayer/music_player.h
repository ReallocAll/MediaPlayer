#pragma once
#include <littlehooker/littlehooker.h>
#include "cpp_string.h"
#include "logger.h"
#include "mc/network.h"
#include "mc/actor.h"
#include "mc/level.h"

void send_music_sound_packet(void);
void music_queue_add_player(long long xuid);
void music_queue_delete_player(long long xuid);
void music_queue_free_queue(void);
