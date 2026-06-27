#pragma once

struct player;

void event_on_server_init_logger(void);
void event_on_server_player_destory(struct player *in_player);
void event_on_server_player_construct(struct player *player);