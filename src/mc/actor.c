#include <mediaplayer/mc/actor.h>

extern struct string string;

struct vec3 *actor_get_pos(struct actor *actor)
{
	return TLCALL("?getPosition@Actor@@UEBAAEBVVec3@@XZ",
					struct vec3 *(*)(struct actor *actor),
					actor);
}

float calc_attack_damage(struct actor *attacker, struct actor *casualty)
{
	return TLCALL("?calculateAttackDamage@Actor@@QEAAMAEAV1@@Z",
					float (*)(struct actor *attacker, struct actor *casualty),
					attacker, casualty);
}

const char *get_name_tag(struct actor *actor)
{
	struct string *cpp_str =
		TLCALL("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			struct string *(*)(struct actor *actor),
			actor);
	return string.c_str(cpp_str);
}

struct player *get_server_player(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt)
{
	return TLCALL("?_getServerPlayer@ServerNetworkHandler@@EEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@W4SubClientId@@@Z",
					struct player *(*)(struct server_network_handler *handler, uintptr_t id, uintptr_t pkt),
					REFERENCE(struct server_network_handler, handler, -16), id, DEREFERENCE(char, pkt, 16));
}

const char *get_player_xuid(struct player *player)
{
	struct string *xuid_cpp_str = string.string("0000000000000000");
	TLCALL("?getXuid@Player@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		struct string *(*)(struct player *, struct string *),
		player, xuid_cpp_str);
	const char *xuid = string.c_str(xuid_cpp_str);
	free(xuid_cpp_str);
	return xuid;
}

unsigned get_entity_type_id(struct actor *actor)
{
	return TLCALL("?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ",
					unsigned (*)(struct actor *actor),
					actor);
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
