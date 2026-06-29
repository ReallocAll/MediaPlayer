#include <stdlib.h>
#include <stdio.h>

#include <libuv/uv.h>
#include <lightbase/mem.h>
#include <lightbase/plugin.h>
#include <cppcompat/cppstr.h>
#include <mediaplayer/mc/symbols.h>
#include <mediaplayer/path.h>

#include <mediaplayer/plugin.h>
#include <mediaplayer/event_handler.h>
#include <mediaplayer/command.h>
#include <mediaplayer/music_player.h>
#include <mediaplayer/video_player.h>
#include <mediaplayer/mc/player.h>
#include <mediaplayer/mc/network.h>
#include <mediaplayer/mc/level.h>
#include <mediaplayer/mc/block.h>
#include <mediaplayer/mc/actor.h>
#include <mediaplayer/mc/structs.h>

SHOOK(on_initialize_logging, void,
      S_DedicatedServer__initializeLogging,
      uintptr_t this)
{
	on_initialize_logging.call(this);
	event_on_server_init_logger();
}

// Constructor for Level
SHOOK(level_construct, struct level *,
      SC_Level__Level, uintptr_t *level, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6, uintptr_t a7, uintptr_t a8, uintptr_t a9, uintptr_t a10, uintptr_t a11, uintptr_t a12, uintptr_t a13, uintptr_t a14, uintptr_t a15, uintptr_t a16, uintptr_t a17, uintptr_t a18, uintptr_t a19)

{
	
	return g_level = level_construct.call(level, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
						  a11, a12, a13, a14, a15, a16, a17, a18, a19);
}

#ifdef __linux__
SHOOK(server_player_construct, struct player *,
      SC_ServerPlayer__ServerPlayer,
      struct player *this, __int64_t a2, __int64_t a3, __int64_t a4, __int64_t a5, __int64_t a6,
      __int64_t a7, char a8, __int64_t a9, __int128_t a10, __int128_t a11, __int64_t *a12, int a13,
      void *a14, struct entity_context *a15)
{
	struct player *player = server_player_construct.call(this, a2, a3, a4, a5, a6, a7, a8, a9,
								 a10, a11, a12, a13, a14, a15);
	event_on_server_player_construct(this);
	return player;
}

SHOOK(server_player_destroy, void,
      SD_ServerPlayer__ServerPlayer,
      struct player *this)
{
	event_on_server_player_destory(this);
	server_player_destroy.call(this);
}
#else

struct Level;

SHOOK(server_player_construct, struct player *,
      SC_ServerPlayer__ServerPlayer,
      struct player *this, struct Level *a2, unsigned long long a3, void *a4, void *a5, int a6, char a7,
      long long a8, long long a9, long long a10, unsigned long long *a11, void *a12, void *a13,
      void **a14, int a15, bool a16, void *a17)
{
	struct player *player = server_player_construct.call(this, a2, a3, a4, a5, a6, a7, a8, a9,
								 a10, a11, a12, a13, a14, a15, a16, a17);
	event_on_server_player_construct(this);
	return player;
}

SHOOK(server_player_destroy, void,
      SD_ServerPlayer__ServerPlayer,
      struct player *this, char a2)
{
	event_on_server_player_destory(this);
	server_player_destroy.call(this, a2);
}
#endif

SHOOK(change_setting_command_setup, void,
      S_ChangeSettingCommand__setup,
      uintptr_t this)
{
	void *cmd_mpm = cppstr_new("mpm");
	void *cmd_mpv = cppstr_new("mpv");
	SYMCALL(S_CommandRegistry__registerCommand,
		void (*)(uintptr_t, void *, const char *, char, short, short),
		this, cmd_mpm, "PediaPlayer music player", 0, 0, 0x80);

	SYMCALL(S_CommandRegistry__registerCommand,
		void (*)(uintptr_t, void *, const char *, char, short, short),
		this, cmd_mpv, "MediaPlayer video player", 0, 0, 0x80);
	cppstr_free(cmd_mpm, true);
	cppstr_free(cmd_mpv, true);
	change_setting_command_setup.call(this);
}

SHOOK(on_player_cmd, void,
      S_ServerNetworkHandler__handle___CommandRequestPacket,
      struct server_network_handler *this, uintptr_t id, uintptr_t pkt)
{
	struct player *player = get_server_player(this, id, pkt);
	const char *cmd = cppstr_str(REFERENCE(void, pkt, 48));
	if (player && !process_cmd(player, cmd))
		return;

	on_player_cmd.call(this, id, pkt);
}

SHOOK(map_item_update, void,
      S_MapItem__update,
      struct map_item *map_item, struct level *level, struct actor *actor, struct map_item_saved_data *map_data)
{
	struct player *player = (struct player *)actor;
	const char *player_xuid = get_player_xuid(player);
	struct video_queue *video_queue_node = video_queue_get_player(player);
	struct screen_pos screen_pos = { 0, 0 };

	if (video_queue_node) {
		play_video(video_queue_node, map_data, &screen_pos);
		free((char *)player_xuid);
	} else {
		map_item_update.call(map_item, level, actor, map_data);
		free((char *)player_xuid);
	}
}

// make MapItemSavedData::tickByBlock always be called
SHOOK(MapItem_doesDisplayPlayerMarkers, bool,
      S_MapItem__doesDisplayPlayerMarkers,
      const struct item_stack *a1)
{
	bool ret = MapItem_doesDisplayPlayerMarkers.call(a1);
	return true;
}

SHOOK(MapItemSavedData_tickByBlock, void,
      S_MapItemSavedData__tickByBlock,
      struct map_item_saved_data *this, const struct block_pos *bl_pos, struct block_source *bs)
{
	int reverse_offset = 0;
	if (bl_pos->x <= start_pos.x && bl_pos->y >= start_pos.y && bl_pos->z <= start_pos.z)
		start_pos = *bl_pos;
	if (bl_pos->x >= end_pos.x && bl_pos->y <= end_pos.y && bl_pos->z >= end_pos.z)
		end_pos = *bl_pos;
	enum direction dire = get_block_face_direction(get_block(bs, bl_pos));
	if (dire == DIRECTION_NEG_Z)
		reverse_offset = abs(start_pos.x - end_pos.x);
	else if (dire == DIRECTION_POS_X)
		reverse_offset = abs(start_pos.z - end_pos.z);

	struct video_queue *video_queue_node = video_queue_get_player(nullptr);
	if (video_queue_node) {
		struct screen_pos screen_pos;
		if (start_pos.x - end_pos.x != 0)
			screen_pos.x = abs(bl_pos->x - start_pos.x - reverse_offset);
		else
			screen_pos.x = abs(bl_pos->z - start_pos.z - reverse_offset);
		screen_pos.y = start_pos.y - bl_pos->y;
		play_video(video_queue_node, this, &screen_pos);
	} else {
		MapItemSavedData_tickByBlock.call(this, bl_pos, bs);
	}
}

SHOOK(on_tick, void,
      S_Level__tick,
      struct level *level)
{
	send_music_sound_packet();
	on_tick.call(level);
}

void init(void)
{
#ifndef __linux__
	char work_path[PATH_MAX_LEN];
	size_t path_size = PATH_MAX_LEN;
	uv_cwd(work_path, &path_size);
	path_init(work_path);
	level_construct.install();
	server_player_construct.install();
	server_player_destroy.install();
	on_initialize_logging.install();
	on_tick.install();
	change_setting_command_setup.install();
	on_player_cmd.install();
	map_item_update.install();
	MapItemSavedData_tickByBlock.install();
	MapItem_doesDisplayPlayerMarkers.install();
#endif
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
