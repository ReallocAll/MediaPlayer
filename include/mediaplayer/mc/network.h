#pragma once
#include <libcutils/libcutils.h>
#include <mediaplayer/mc/symbols.h>
#include <mediaplayer/logger.h>
#include "position.h"
#include "actor.h"
#include "player.h"
#include "structs.h"

enum boss_bar_event_type {
	BOSS_BAR_DISPLAY,
	BOSS_BAR_UPDATE,
	BOSS_BAR_HIDE,
};

enum text_type {
    TEXT_TYPE_RAW,
    TEXT_TYPE_CHAT,
    TEXT_TYPE_TRANSLATION,
    TEXT_TYPE_POPUP,
    TEXT_TYPE_JUKEBOX_POPUP,
    TEXT_TYPE_TIP,
    TEXT_TYPE_SYSTEM,
    TEXT_TYPE_WHISPER,
    TEXT_TYPE_ANNOUNCEMENT,
    TEXT_TYPE_JSON_WHISPER,
    TEXT_TYPE_JSON
};

uintptr_t create_packet(int type);
void send_network_packet(struct player *player, uintptr_t pkt);

void send_play_sound_packet(struct player *player, const char *sound_name,
			 				struct vec3 *pos, float volume, float pitch);
void send_text_packet(struct player *player, enum text_type mode, const char *msg);
void send_boss_event_packet(struct player *player, const char *name, 
							float per, enum boss_bar_event_type type);
