#include <mediaplayer/plugin.h>

extern struct string string;
extern struct level *g_level;

TLHOOK(on_initialize_logging, void,
        "?initializeLogging@DedicatedServer@@AEAAXXZ",
        uintptr_t _this)
{
    on_initialize_logging.original(_this);
    server_logger("MediaPlayer Loaded!", INFO);
}

// Constructor for Level		
TLHOOK(level_construct, struct level*,
    "??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NW4SubClientId@@AEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEBV?$not_null@V?$NonOwnerPointer@VIEntityRegistryOwner@@@Bedrock@@@2@V?$WeakRefT@UEntityRefTraits@@@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@VItemRegistryRef@@V?$weak_ptr@VBlockTypeRegistry@@@4@4V?$optional@VDimensionDefinitionGroup@@@4@@Z",
    struct level* _this, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16, uintptr_t a17)
{
    return g_level = level_construct.original(_this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17);
}

TLHOOK(on_player_join, void,
    "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVSetLocalPlayerAsInitializedPacket@@@Z",
    struct server_network_handler *_this, uintptr_t id, uintptr_t pkt)
{
    struct player *player = get_server_player(_this, id, pkt);
    music_queue_add_player(atoll(get_player_xuid(player)));
    on_player_join.original(_this, id, pkt);
}

TLHOOK(on_tick, void,
    "?tick@Level@@UEAAXXZ",
    struct level *level)
{
    send_music_sound_packet();
    on_tick.original(level);
}

bool init_hooks(void)
{
    level_construct.init(&level_construct);

    on_initialize_logging.init(&on_initialize_logging);
    on_player_join.init(&on_player_join);
    on_tick.init(&on_tick);

    lh_enable_all_hook();
    return true;
}
