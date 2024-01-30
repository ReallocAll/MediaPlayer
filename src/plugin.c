#include <mediaplayer/plugin.h>


#ifndef __linux__
FARPROC hook_func_address = 0;
FARPROC unhook_func_address = 0;
FARPROC dlsym_func_address = 0;
#endif


THOOK(on_initialize_logging, void,
		S_DedicatedServer__initializeLogging,
		uintptr_t this)
{
	on_initialize_logging.original(this);
	server_logger(LOG_LEVEL_INFO, "MediaPlayer Loaded!%s%s", PLUGIN_VERSION_MSG, PLUGIN_VERSION);
}

// Constructor for Level
THOOK(level_construct, struct level *,
	SC_Level__Level,
	struct level *level, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5,
	uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11,
	uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16, uintptr_t a17,
	uintptr_t a18, uintptr_t a19)
{
	return g_level = level_construct.original(level, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
				a11, a12, a13, a14, a15, a16, a17, a18, a19);
}

#ifdef __linux__
THOOK(server_player_construct, struct player *,
	SC_ServerPlayer__ServerPlayer,
	struct player *this, __int64_t a2, __int64_t a3, __int64_t a4, __int64_t a5, __int64_t a6,
	__int64_t a7, char a8, __int64_t a9, __int128_t a10, __int128_t a11, __int64_t *a12, int a13,
        void *a14, struct entity_context *a15)
{
	struct player *player = server_player_construct.original(this, a2, a3, a4, a5, a6, a7, a8, a9,
								 			 					a10, a11, a12, a13, a14, a15);
	player_list_add(player);
	return player;
}

THOOK(server_player_destroy, void,
	SD_ServerPlayer__ServerPlayer,
	struct player *this)
{
	player_list_delete(this);
	video_queue_delete_player(this);
	music_queue_delete_player(this);
	server_player_destroy.original(this);
}
#else
THOOK(server_player_construct, struct player *,
	SC_ServerPlayer__ServerPlayer,
	struct player *this, struct Level *a2,
	__int64_t a3, __int64_t a4, __int64_t a5, void *a6, __int64_t a7, char a8, __int64_t a9,
	__int128_t *a10, void *a11, void *a12, __int64_t *a13, int a14, bool a15, struct entity_context *a16)
{
	struct player *player = server_player_construct.original(this, a2, a3, a4, a5, a6, a7, a8, a9,
								 			 					a10, a11, a12, a13, a14, a15, a16);
	player_list_add(player);
	return player;
}

THOOK(server_player_destroy, void,
	SD_ServerPlayer__ServerPlayer,
	struct player *this, char a2)
{
	player_list_delete(this);
	video_queue_delete_player(this);
	music_queue_delete_player(this);
	server_player_destroy.original(this, a2);
}
#endif


THOOK(ChangeSettingCommand_setup, void,
	S_ChangeSettingCommand__setup,
	uintptr_t this)
{
	void *cmd_mpm = NULL;
	void *cmd_mpv = NULL;
	std_string_string(&cmd_mpm, "mpm");
	std_string_string(&cmd_mpv, "mpv");
	SYMCALL(S_CommandRegistry__registerCommand,
		void (*)(uintptr_t, void *, const char *, char, short, short),
		this, cmd_mpm, "PediaPlayer music player", 0, 0, 0x80);
	SYMCALL(S_CommandRegistry__registerCommand,
		void (*)(uintptr_t, void *, const char *, char, short, short),
		this, cmd_mpv, "MediaPlayer video player", 0, 0, 0x80);
	std_string_destroy(cmd_mpm, true);
	std_string_destroy(cmd_mpv, true);
	ChangeSettingCommand_setup.original(this);
}

THOOK(on_player_cmd, void,
	S_ServerNetworkHandler__handle___CommandRequestPacket,
	struct server_network_handler *this, uintptr_t id, uintptr_t pkt)
{
	struct player *player = get_server_player(this, id, pkt);
	const char *cmd = std_string_c_str(REFERENCE(void, pkt, 48));
	if (player && !process_cmd(player, cmd))
		return;

	on_player_cmd.original(this, id, pkt);
}


THOOK(map_item_update, void,
	S_MapItem__update,
	struct map_item *map_item, struct level *level, struct actor *actor, struct map_item_saved_data *map_data)
{
	struct player *player = (struct player *)actor;
	const char *player_xuid = get_player_xuid(player);
	struct video_queue *video_queue_node = video_queue_get_player(player);
	struct screen_pos screen_pos = {0, 0};

	if (video_queue_node) {
		play_video(video_queue_node, map_data, &screen_pos);
	} else {
		map_item_update.original(map_item, level, actor, map_data);
	}
}

// make MapItemSavedData::tickByBlock always be called
THOOK(MapItem_doesDisplayPlayerMarkers, bool,
	S_MapItem__doesDisplayPlayerMarkers,
	const struct item_stack *a1)
{
	bool ret = MapItem_doesDisplayPlayerMarkers.original(a1);
	return true;
}

THOOK(MapItemSavedData_tickByBlock, void,
	S_MapItemSavedData__tickByBlock,
	struct map_item_saved_data *this, const struct block_pos *bl_pos, struct block_source *bs)
{
	int reverse_offset = 0;
	if (bl_pos->x <= start_pos.x && bl_pos->y >= start_pos.y && bl_pos->z <= start_pos.z) 
		start_pos = *bl_pos;
	if (bl_pos->x >= end_pos.x && bl_pos->y <= end_pos.y && bl_pos->z >= end_pos.z) 
		end_pos = *bl_pos;
	struct block *bl =
		SYMCALL(S_BlockSource__getBlock,
				struct block *(*)(struct block_source *, const struct block_pos *),
				bs, bl_pos);
	enum direction dire =
		SYMCALL(S_FaceDirectionalBlock__getFacingDirection,
			unsigned char (*)(const struct block *, bool),
			bl, false);
	if (dire == DIRECTION_NEG_Z)
		reverse_offset = abs(start_pos.x - end_pos.x);
	else if (dire == DIRECTION_POS_X)
		reverse_offset = abs(start_pos.z - end_pos.z);

	struct video_queue *video_queue_node = video_queue_get_player(NULL);
	if (video_queue_node) {
		struct screen_pos screen_pos;
		if (start_pos.x - end_pos.x != 0)
			screen_pos.x = abs(bl_pos->x - start_pos.x - reverse_offset);
		else
			screen_pos.x = abs(bl_pos->z - start_pos.z - reverse_offset);
		screen_pos.y = start_pos.y - bl_pos->y;
		play_video(video_queue_node, this, &screen_pos);
	} else {
		MapItemSavedData_tickByBlock.original(this, bl_pos, bs);
	}
}

THOOK(on_tick, void,
	S_Level__tick,
	struct level *level)
{
	send_music_sound_packet();
	on_tick.original(level);
}

void init(void)
{
	#ifndef __linux__
	hook_func_address = GetProcAddress(GetModuleHandleA("PreLoader"), "pl_hook");
	unhook_func_address = GetProcAddress(GetModuleHandleA("PreLoader"), "pl_unhook");
	dlsym_func_address = GetProcAddress(GetModuleHandleA("PreLoader"), "pl_resolve_symbol");

	level_construct.install();
	server_player_construct.install();
	server_player_destroy.install();
	on_initialize_logging.install();
	on_tick.install();
	ChangeSettingCommand_setup.install();
	on_player_cmd.install();
	map_item_update.install();
	MapItemSavedData_tickByBlock.install();
	MapItem_doesDisplayPlayerMarkers.install();
	#endif
	create_plugin_dir();
}

void create_plugin_dir(void)
{
	// also init global path variables
	char work_path[4096];
	size_t path_size = 4096;
	uv_cwd(work_path, &path_size);

	sprintf(data_path, "%s/plugins/MediaPlayer", work_path);
	sprintf(data_path_nbs, "%s/nbs", data_path);
	sprintf(data_path_video, "%s/video", data_path);

	make_directory(data_path);
	make_directory(data_path_nbs);
	make_directory(data_path_video);
}

#ifndef __linux__

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		init();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return true;
}

#endif
