#include <mediaplayer/mc/player.h>

struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt)
{
	return SYMCALL("?_getServerPlayer@ServerNetworkHandler@@EEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@W4SubClientId@@@Z",
					struct player *(*)(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt),
					REFERENCE(struct server_network_handler, handler, -16), id, DEREFERENCE(char, pkt, 16));
}

const char *get_player_xuid(struct player *player)
{
	struct string *xuid_cpp_str = std_string_string("0000000000000000");
	SYMCALL("?getXuid@Player@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		struct string *(*)(struct player *, struct string *),
		player, xuid_cpp_str);
	const char *xuid = std_string_c_str(xuid_cpp_str);
	free(xuid_cpp_str);
	return xuid;
}

bool is_player_init(struct player *player)
{
	if (!player)
		return false;
	return SYMCALL("?isPlayerInitialized@ServerPlayer@@UEBA_NXZ",
					bool (*)(struct player *player),
					player);
}
