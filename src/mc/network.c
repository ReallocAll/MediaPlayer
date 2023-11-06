#include <mediaplayer/mc/network.h>

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
	uintptr_t pkt = create_packet(86);
	void *sound_name_sstr = NULL;
	std_string_string(&sound_name_sstr, sound_name);
	
	SYMCALL(SC_PlaySoundPacket__PlaySoundPacket,
		uintptr_t (*)(uintptr_t pkt, void *sound_name, struct vec3 *pos, float volume, float pitch),
		pkt, sound_name_sstr, pos, volume, pitch);

	send_network_packet(player, pkt);
	std_string_destroy(sound_name_sstr, true);
}


uintptr_t create_text_packet(enum text_type type, struct player *player, const char *msg)
{
	void *author = NULL;
	void *message = NULL;
	void *xuid = NULL;
	void *platform_id = NULL;
	std_string_string(&author, get_name_tag((struct actor *)player));
	std_string_string(&message, msg);
	std_string_string(&xuid, get_player_xuid(player));
	std_string_string(&platform_id, "");
	uintptr_t pkt = create_packet(9);
	#ifdef __linux__
	uintptr_t params[2];
	SYMCALL(S_TextPacket__TextPacket,
			uintptr_t (*)(uintptr_t pkt, enum text_type type, void *author, void *message, void *params, bool localized, void *xuid, void *platform_id),
			pkt, type, author, message, &params, 0, xuid, platform_id);
	#else
	DEREFERENCE(int, pkt, 48) = type;
 	memcpy((void *)(pkt + 56), author, 32);
 	memcpy((void *)(pkt + 88), message, 32);
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
	uintptr_t pkt = create_packet(74);
	uintptr_t unique_id = DEREFERENCE(uintptr_t, get_or_create_unique_id((struct actor *)player), 0);
	void *name_sstr = NULL;
	std_string_string(&name_sstr, name);
	DEREFERENCE(uintptr_t, pkt, 56) = unique_id;
	DEREFERENCE(int, pkt, 72) = type;
 	memcpy((void *)(pkt + 80), name_sstr, 32);
	DEREFERENCE(float, pkt, 112) = per * 10.0f;
	send_network_packet(player, pkt);
	std_string_destroy(name_sstr, true);
}
