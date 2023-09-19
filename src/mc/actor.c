#include <mediaplayer/mc/actor.h>


struct vec3 *actor_get_pos(struct actor *actor)
{
	return TLCALL("?getPosition@Actor@@UEBAAEBVVec3@@XZ",
					struct vec3 *(*)(struct actor *actor),
					actor);
}

const char *get_name_tag(struct actor *actor)
{
	struct string *cpp_str =
		TLCALL("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			struct string *(*)(struct actor *actor),
			actor);
	return std_string_c_str(cpp_str);
}

uintptr_t get_or_create_unique_id(struct actor *actor)
{
	return TLCALL("?getOrCreateUniqueID@Actor@@QEBAAEBUActorUniqueID@@XZ",
					uintptr_t (*)(struct actor *actor),
					actor);
}
