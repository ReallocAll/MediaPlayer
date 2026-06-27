#include <string.h>

#include <libcutils/libcutils.h>
#include <mediaplayer/mc/symbols.h>
#include <mediaplayer/mc/player.h>
#include <stb/stb_ds.h>


struct player **g_player_list = nullptr;


struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt)
{
	#ifdef __linux__
	int handler_offset = 0;
	#else
	int handler_offset = -16;
	#endif
	return SYMCALL(S_ServerNetworkHandler___getServerPlayer,
					struct player *(*)(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt),
					REFERENCE(struct server_network_handler, handler, handler_offset), id, DEREFERENCE(char, pkt, 16));
}


const char *get_player_xuid(struct player *player)
{
	void *xuid_sstr = nullptr;
	const char *xuid;

	std_string_string(&xuid_sstr, "00000000000000000");
	SYMCALL(S_Player__getXuid,
		void *(*)(struct player *, void *),
		player, xuid_sstr);
	xuid = strdup(std_string_c_str(xuid_sstr));
	std_string_destroy(xuid_sstr, true);
	return xuid;
}


bool is_player_init(struct player *player)
{
	return player && player_list_get(player) != -1;
}


int player_list_get(struct player *player)
{
	int len = (int)arrlen(g_player_list);
	for (int i = 0; i < len; i++) {
		if (g_player_list[i] == player)
			return i;
	}
	return -1;
}


void player_list_add(struct player *player)
{
	arrput(g_player_list, player);
}


void player_list_delete(struct player *player)
{
	int i = player_list_get(player);
	if (i != -1)
		arrdelswap(g_player_list, i);
}
