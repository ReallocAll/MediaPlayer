#pragma once
#include <littlehooker/littlehooker.h>
#include <mediaplayer/cpp_string.h>
#include "position.h"
#include "actor.h"

enum boss_bar_event_type {
	BOSS_BAR_DISPLAY,
	BOSS_BAR_UPDATE,
	BOSS_BAR_HIDE,
};

struct server_network_handler;

inline uintptr_t create_packet(int type);
inline void send_network_packet(struct player *player, uintptr_t pkt);

void send_play_sound_packet(struct player *player, const char *sound_name,
			 				struct vec3 *pos, float volume, float pitch);
void send_text_packet(struct player *player, int mode, const char *msg);
void send_boss_event_packet(struct player *player, const char *name, 
							float per, enum boss_bar_event_type type);
