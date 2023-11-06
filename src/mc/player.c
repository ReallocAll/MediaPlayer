#include <mediaplayer/mc/player.h>


struct player *(*g_player_list) = NULL;
int g_player_list_size = 0;


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
	void *xuid_sstr = NULL;
	std_string_string(&xuid_sstr, "00000000000000000");
	SYMCALL(S_Player__getXuid,
		void *(*)(struct player *, void *),
		player, xuid_sstr);
	const char *xuid = strdup(std_string_c_str(xuid_sstr));
	std_string_destroy(xuid_sstr, true);
	return xuid;
}


bool is_player_init(struct player *player)
{
	if (!player || player_list_get(player) == -1)
		return false;
	return true;
}


int player_list_get(struct player *player)
{
	if (g_player_list_size == 0)
		return -1;
	for (int i = 0; i < g_player_list_size; i++) {
		if (g_player_list[i] == player)
			return i;
	}
	return -1;
}


void player_list_add(struct player *player)
{
	if (g_player_list_size == 0) {
		g_player_list_size = 1;
		g_player_list = malloc(sizeof(struct player *));
		g_player_list[0] = player;
	} else {
		g_player_list_size++;
		g_player_list = realloc(g_player_list, g_player_list_size * sizeof(struct player *));
		g_player_list[g_player_list_size - 1] = player;
	}
}


void player_list_delete(struct player *player)
{
	if (g_player_list_size == 0) 
		return;
	int i = player_list_get(player);
	if (i != -1) {
		g_player_list_size--;
		g_player_list[i] = g_player_list[g_player_list_size];
		g_player_list[g_player_list_size] = NULL;
		g_player_list = realloc(g_player_list, g_player_list_size * sizeof(struct player *));
	}
}
