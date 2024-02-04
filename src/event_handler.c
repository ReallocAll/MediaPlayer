#include <mediaplayer/event_handler.h>

extern xr_dynamic_array_info g_player_array_0_info;
extern xr_dynamic_array_info g_offline_player_array_0_info;
extern struct player_music_info *g_player_array_0;
extern struct player_music_info *g_offline_player_array_0;

void event_on_server_init_logger()
{
	server_logger(LOG_LEVEL_INFO, "MediaPlayer Loaded!%s%s", PLUGIN_VERSION_MSG, PLUGIN_VERSION);
	g_player_array_0 = xr_new_dynamic_array(sizeof(struct player_music_info), &g_player_array_0_info);
	g_offline_player_array_0 = xr_new_dynamic_array(sizeof(struct player_music_info), &g_offline_player_array_0_info);
}

void event_on_server_player_construct(struct player *in_player)
{
	player_list_add(in_player);
	music_player_player_online(in_player);
}

void event_on_server_player_destory(struct player *in_player)
{
	player_list_delete(in_player);
	video_queue_delete_player(in_player);
    music_player_player_offline(in_player);
}