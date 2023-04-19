#include <mediaplayer/mc/actor.h>


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
	return std_string_c_str(cpp_str);
}

unsigned get_entity_type_id(struct actor *actor)
{
	return TLCALL("?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ",
					unsigned (*)(struct actor *actor),
					actor);
}

uintptr_t get_unique_id(struct actor *actor)
{
	return TLCALL("?getUniqueID@Actor@@QEBAAEBUActorUniqueID@@XZ",
					uintptr_t (*)(struct actor *actor),
					actor);
}
