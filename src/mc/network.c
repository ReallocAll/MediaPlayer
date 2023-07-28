#include <mediaplayer/mc/network.h>


inline uintptr_t create_packet(int type)
{
	uintptr_t pkt[2];
	TLCALL("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		void (*)(uintptr_t [2], int),
		pkt, type);
	return *pkt;
}

inline void send_network_packet(struct player *player, uintptr_t pkt)
{
	TLCALL("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z",
		void (*)(struct player *player, uintptr_t pkt),
		player, pkt);
}

void send_play_sound_packet(struct player *player, const char *sound_name,
			 				struct vec3 *pos, float volume, float pitch)
{
	uintptr_t pkt = create_packet(86);

	// Never dereference struct string.
	// Please use functions like memcpy to manipulate data
	struct string *sound_name_cpp_str = std_string_string(sound_name);
	// TLCALL("??0PlaySoundPacket@@QEAA@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVVec3@@MM@Z",
	// 	uintptr_t (*)(uintptr_t pkt, struct string *sound_name,
	// 				 	struct vec3 *pos, float volume, float pitch),
	// 	pkt, sound_name_cpp_str, pos, volume, pitch);

	// faster method for writing data to packet
 	memcpy((void *)(pkt + 48), sound_name_cpp_str, 32);

	DEREFERENCE(int, pkt, 80) = (int)(pos->x * 8.0F);
	DEREFERENCE(int, pkt, 84) = (int)(pos->y * 8.0F);
	DEREFERENCE(int, pkt, 88) = (int)(pos->z * 8.0F);
	DEREFERENCE(float, pkt, 92) = volume;
	DEREFERENCE(float, pkt, 96) = pitch;
	send_network_packet(player, pkt);
	free(sound_name_cpp_str);
}

void send_text_packet(struct player *player, int mode, const char *msg)
{
	uintptr_t pkt = create_packet(9);
	const char *player_name = get_name_tag((struct actor *)player);
	struct string *player_name_cpp_str = std_string_string(player_name);
	struct string *msg_cpp_str = std_string_string(msg);
	DEREFERENCE(int, pkt, 48) = mode;
 	memcpy((void *)(pkt + 56), player_name_cpp_str, 32);
 	memcpy((void *)(pkt + 88), msg_cpp_str, 32);
	send_network_packet(player, pkt);
	free(player_name_cpp_str);
	free(msg_cpp_str);
}

void send_boss_event_packet(struct player *player, const char *name,
							float per, enum boss_bar_event_type type)
{
	uintptr_t pkt = create_packet(74);
	uintptr_t unique_id = DEREFERENCE(uintptr_t, get_or_create_unique_id((struct actor *)player), 0);
	struct string *name_cpp_str = std_string_string(name);
	DEREFERENCE(uintptr_t, pkt, 56) = unique_id;
	DEREFERENCE(int, pkt, 72) = type;
 	memcpy((void *)(pkt + 80), name_cpp_str, 32);
	DEREFERENCE(float, pkt, 112) = per;
	send_network_packet(player, pkt);
	free(name_cpp_str);
}
