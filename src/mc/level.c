#include <mediaplayer/mc/level.h>

extern struct string string;

struct level *g_level = NULL;

struct player *get_player_by_xuid(struct level *level, const char *xuid)
{
    struct string *xuid_cpp_str = string.string(xuid);
    struct player *player = 
        TLCALL("?getPlayerByXuid@Level@@UEBAPEAVPlayer@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
            struct player *(*)(struct level *level, struct string *xuid),
            level, xuid_cpp_str);
    free(xuid_cpp_str);
    return player;
}
