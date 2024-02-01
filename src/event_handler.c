#include <mediaplayer/event_handler.h>

void event_on_player_left(struct player *in_player)
{
    music_player_player_offline(in_player);
}