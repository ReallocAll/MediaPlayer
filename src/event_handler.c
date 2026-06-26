#include <mediaplayer/event_handler.h>
#include <mediaplayer/plugin.h>
#include <mediaplayer/mc/player.h>
#include <mediaplayer/music_player.h>
#include <mediaplayer/video_player.h>

extern player_music_t *g_player_arr;
extern player_music_t *g_offline_player_arr;

void event_on_server_init_logger(void)
{
	server_logger(LOG_LEVEL_INFO, "MediaPlayer Loaded!%s%s", PLUGIN_VERSION_MSG, PLUGIN_VERSION);
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