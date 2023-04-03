#pragma once
#include <littlehooker/littlehooker.h>
#include <mediaplayer/cpp_string.h>
#include "position.h"

struct server_network_handler;

inline uintptr_t create_packet(int type);
inline void send_network_packet(struct player *player, uintptr_t pkt);

void send_play_sound_packet(struct player *player, const char *sound_name,
			 				struct vec3 *pos, float volume, float pitch);
