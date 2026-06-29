#include <string.h>
#include <stdlib.h>

#include <lightbase/mem.h>
#include <cppcompat/cppstr.h>
#include <mediaplayer/mc/symbols.h>
#include <mediaplayer/mc/network.h>
#include <mediaplayer/mc/player.h>
#include <mediaplayer/mc/actor.h>

uintptr_t create_packet(int type)
{
	uintptr_t pkt[2];
	SYMCALL(S_MinecraftPackets__createPacket,
		void (*)(uintptr_t [2], int type),
		pkt, type);
	return *pkt;
}


void send_network_packet(struct player *player, uintptr_t pkt)
{
	SYMCALL(S_ServerPlayer__sendNetworkPacket,
			void (*)(struct player *player, uintptr_t pkt),
			player, pkt);
}

void send_play_sound_packet(struct player *player, const char *sound_name,
			 				struct vec3 *pos, float volume, float pitch)
{
	uintptr_t pkt = create_packet(PKT_ID_PLAY_SOUND);
	void *sstr = cppstr_new(sound_name);

	SYMCALL(SC_PlaySoundPacket__PlaySoundPacket,
		uintptr_t (*)(uintptr_t pkt, void *sound_name, struct vec3 *pos, float volume, float pitch),
		pkt, sstr, pos, volume, pitch);

	send_network_packet(player, pkt);
	cppstr_free(sstr, true);
}


uintptr_t create_text_packet(enum text_type type, struct player *player, const char *msg)
{
	uintptr_t pkt = create_packet(PKT_ID_TEXT);
	#ifdef __linux__
	void *author  = cppstr_new(get_name_tag((struct actor *)player));
	void *message = cppstr_new(msg);
	const char *xuid_str = get_player_xuid(player);
	void *xuid        = cppstr_new(xuid_str);
	void *platform_id = cppstr_new("");
	free((char *)xuid_str);
	uintptr_t params[2];
	SYMCALL(S_TextPacket__TextPacket,
			uintptr_t (*)(uintptr_t pkt, enum text_type type, void *author, void *message, void *params, bool localized, void *xuid, void *platform_id),
			pkt, type, author, message, &params, 0, xuid, platform_id);
	cppstr_free(author, true);
	cppstr_free(message, true);
	cppstr_free(xuid, true);
	cppstr_free(platform_id, true);
	#else
	cppstr_place((void *)(pkt + TEXTPKT_AUTHOR_OFFSET),
		get_name_tag((struct actor *)player));
	cppstr_place((void *)(pkt + TEXTPKT_MESSAGE_OFFSET), msg);
	const char *xuid_str = get_player_xuid(player);
	cppstr_place((void *)(pkt + 192), xuid_str);
	free((char *)xuid_str);
	cppstr_place((void *)(pkt + 224), "");
	DEREFERENCE(int, pkt, TEXTPKT_TYPE_OFFSET) = type;
	#endif
	return pkt;
}

void send_text_packet(struct player *player, enum text_type type, const char *msg)
{
	uintptr_t pkt = create_text_packet(type, player, msg);
	send_network_packet(player, pkt);
}


void send_boss_event_packet(struct player *player, const char *name,
							float per, enum boss_bar_event_type type)
{
	uintptr_t pkt = create_packet(PKT_ID_BOSS_EVENT);
	uintptr_t unique_id = DEREFERENCE(uintptr_t, get_or_create_unique_id((struct actor *)player), 0);

	cppstr_place((void *)(pkt + BOSSPKT_NAME_OFFSET), name);
	DEREFERENCE(uintptr_t, pkt, BOSSPKT_ENTITY_ID_OFFSET) = unique_id;
	DEREFERENCE(int, pkt, BOSSPKT_EVENT_TYPE_OFFSET) = type;
	DEREFERENCE(float, pkt, BOSSPKT_PROGRESS_OFFSET) = per;

	send_network_packet(player, pkt);
}
