#include <mediaplayer/mc/player.h>

struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt)
{
	return TLCALL("?_getServerPlayer@ServerNetworkHandler@@EEAAPEAVServerPlayer@@"
					"AEBVNetworkIdentifier@@W4SubClientId@@@Z",
					struct player *(*)(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt),
					REFERENCE(struct server_network_handler, handler, -16), id, DEREFERENCE(char, pkt, 16));
}

const char *get_player_xuid(struct player *player)
{
	struct string *xuid_cpp_str = std_string_string("0000000000000000");
	TLCALL("?getXuid@Player@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		struct string *(*)(struct player *, struct string *),
		player, xuid_cpp_str);
	const char *xuid = std_string_c_str(xuid_cpp_str);
	free(xuid_cpp_str);
	return xuid;
}

bool is_player(void *ptr)
{
	if (ptr == NULL)
		return false;
	if (get_entity_type_id((struct actor *)ptr) != 1)
		return false;
	return true;
}

bool is_player_init(struct player *player)
{
	return TLCALL("?isPlayerInitialized@ServerPlayer@@UEBA_NXZ",
					bool (*)(struct player *player),
					player);
}
