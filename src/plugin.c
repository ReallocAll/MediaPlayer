#include <mediaplayer/plugin.h>


TLHOOK(on_initialize_logging, void,
		"?initializeLogging@DedicatedServer@@AEAAXXZ",
		uintptr_t this)
{
	on_initialize_logging.original(this);
	server_logger("MediaPlayer Loaded!", INFO);
}

// Constructor for Level
TLHOOK(level_construct, struct level *,
	"??0Level@@QEAA@AEBV?$not_null@V?$NonOwnerPointer@VSoundPlayerInterface@@@Bedrock@@@gsl@@V?$unique_ptr@VLevelStorage@@U?$default_delete@VLevelStorage@@@std@@@std@@V?$unique_ptr@VLevelLooseFileStorage@@U?$default_delete@VLevelLooseFileStorage@@@std@@@4@AEAVIMinecraftEventing@@_NW4SubClientId@@AEAVScheduler@@V?$not_null@V?$NonOwnerPointer@VStructureManager@@@Bedrock@@@2@AEAVResourcePackManager@@AEBV?$not_null@V?$NonOwnerPointer@VIEntityRegistryOwner@@@Bedrock@@@2@V?$WeakRefT@UEntityRefTraits@@@@V?$unique_ptr@VBlockComponentFactory@@U?$default_delete@VBlockComponentFactory@@@std@@@4@V?$unique_ptr@VBlockDefinitionGroup@@U?$default_delete@VBlockDefinitionGroup@@@std@@@4@VItemRegistryRef@@V?$weak_ptr@VBlockTypeRegistry@@@4@44V?$optional@VDimensionDefinitionGroup@@@4@@Z",
	struct level *level, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16, uintptr_t a17, uintptr_t a18)
{
	return g_level = level_construct.original(level, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18);
}

TLHOOK(change_setting_command_setup, void,
	"?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",
	uintptr_t this)
{
	struct string *cmd_music = std_string_string("mpm");
	struct string *cmd_video = std_string_string("mpv");
	TLCALL("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
		void (*)(uintptr_t, struct string *, const char *, char, short, short),
		this, cmd_music, "mediaplayer music", 0, 0, 0x80);
	TLCALL("?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z",
		void (*)(uintptr_t, struct string *, const char *, char, short, short),
		this, cmd_video, "mediaplayer video", 0, 0, 0x80);
	free(cmd_music);
	free(cmd_video);
	change_setting_command_setup.original(this);
}

TLHOOK(on_player_cmd, void,
	"?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandRequestPacket@@@Z",
	struct server_network_handler *this, uintptr_t id, uintptr_t pkt)
{
	struct player *player = get_server_player(this, id, pkt);
	const char *cmd = std_string_c_str(REFERENCE(struct string, pkt, 48));
	if (player && !process_cmd(player, cmd))
		return;

	on_player_cmd.original(this, id, pkt);
}

TLHOOK(map_item_update, void,
	"?update@MapItem@@QEBAXAEAVLevel@@AEAVActor@@AEAVMapItemSavedData@@@Z",
	struct map_item *map_item, struct level *level, struct actor *actor, struct map_item_saved_data *map_data)
{
	struct player *player = (struct player *)actor;
	const char *player_xuid = get_player_xuid(player);
	struct video_queue *video_queue_node = video_queue_get_player(atoll(player_xuid));
	if (video_queue_node) {
		play_video(video_queue_node, map_data);
	} else {
		map_item_update.original(map_item, level, actor, map_data);
	}
}

TLHOOK(on_tick, void,
	"?tick@Level@@UEAAXXZ",
	struct level *level)
{
	send_music_sound_packet();
	on_tick.original(level);
}

bool using_ll_preloader_api = false;

bool check_ll_preloader(void)
{
    FILE *fp = fopen("LLPreloader.dll", "rb");
	if (fp) {
		fclose(fp);
		puts("00:00:00 INFO [MediaPlayer] The LLPreLoader is detected and is using the HookAPI it provides.");
		using_ll_preloader_api = true;
		return true;
	}
	return false;
}

bool init_hooks(void)
{
	level_construct.init(&level_construct);

	on_initialize_logging.init(&on_initialize_logging);
	on_tick.init(&on_tick);
	change_setting_command_setup.init(&change_setting_command_setup);
	on_player_cmd.init(&on_player_cmd);
	map_item_update.init(&map_item_update);
	return true;
}

void create_plugin_dir(void)
{
	// also init global path variables
	GetCurrentDirectoryA(260, bds_path);

	sprintf(data_path, "%s\\plugins\\MediaPlayer", bds_path);
	sprintf(data_path_nbs, "%s\\nbs", data_path);
	sprintf(data_path_video, "%s\\video", data_path);

	CreateDirectoryA(data_path, NULL);
	CreateDirectoryA(data_path_nbs, NULL);
	CreateDirectoryA(data_path_video, NULL);
}

bool load_plugin(void)
{
	create_plugin_dir();
	check_ll_preloader();
	if (!using_ll_preloader_api && !lh_init()) {
		puts("LittleHooker init failed\n");
		return false;
	}
	init_hooks();

	if (!using_ll_preloader_api)
		lh_enable_all_hook();

	return true;
}

bool unload_plugin(void)
{
	if (!using_ll_preloader_api)
		lh_uninit();
	return true;
}
