#include <mediaplayer/mc/actor.h>


struct vec3 *actor_get_pos(struct actor *actor)
{
	return SYMCALL(S_Actor__getPosition,
					struct vec3 *(*)(struct actor *actor),
					actor);
}

const char *get_name_tag(struct actor *actor)
{
	struct string *sstr =
		SYMCALL(S_Actor__getNameTag,
			struct string *(*)(struct actor *actor),
			actor);
	return std_string_c_str(sstr);
}

uintptr_t get_or_create_unique_id(struct actor *actor)
{
	return SYMCALL(S_Actor__getOrCreateUniqueID,
					uintptr_t (*)(struct actor *actor),
					actor);
}
