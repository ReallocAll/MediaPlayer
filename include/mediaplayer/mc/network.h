#pragma once
#include <stdint.h>
#include "position.h"

/* Minecraft packet IDs */
#define PKT_ID_PLAY_SOUND    86
#define PKT_ID_TEXT           9
#define PKT_ID_BOSS_EVENT    74

/* TextPacket field offsets (MSVC, 32-bit std::string) */
#define TEXTPKT_TYPE_OFFSET      48
#define TEXTPKT_AUTHOR_OFFSET    56
#define TEXTPKT_MESSAGE_OFFSET   88
#define STD_STRING_SZ            32

/* BossEventPacket field offsets (MSVC, 32-bit std::string) */
#define BOSSPKT_ENTITY_ID_OFFSET  56
#define BOSSPKT_EVENT_TYPE_OFFSET 72
#define BOSSPKT_NAME_OFFSET       80
#define BOSSPKT_PROGRESS_OFFSET  112

struct server_network_handler;
struct player;

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
