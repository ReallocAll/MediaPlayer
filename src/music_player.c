#include <mediaplayer/music_player.h>
#include <xiziya_r/misc/xr_dynamic_array.h>

extern char data_path[4096];

xr_dynamic_array_info g_player_array_0_info;
xr_dynamic_array_info g_offline_player_array_0_info;
xr_dynamic_array_info g_note_array_0_info;

struct player_music_info *g_player_array_0 = 0;
struct player_music_info *g_offline_player_array_0 = 0;
struct music_note_info * g_note_array_0 = 0;

// struct music_queue_node *music_queue_head = NULL; //will be remove in few days...

struct note_queue_node *generate_note_queue(FILE *fp, time_t *total_time)
{
	struct note_queue_node *note_node_head = NULL;
	struct note_queue_node *note_node_tail = NULL;

	struct nbs_header *nbs_header = nbs_parse_header(fp);
	struct nbs_notes *notes_head = nbs_parse_notes(fp, nbs_header->version);
	struct nbs_layers *layers_head = nbs_parse_layers(fp, nbs_header->song_layers, nbs_header->version);
	struct nbs_instruments *instruments_head = nbs_parse_instruments(fp, nbs_header->song_layers, nbs_header->version);

	struct nbs_notes *notes = notes_head;
	float time_per_tick = (float)(20.0f / nbs_header->tempo * 50.0f);
	while (notes != NULL) {

		struct nbs_layers *layers_node = layers_head;
		for (int layer_count = 0; layer_count < notes->layer; layer_count++) {
			layers_node = layers_node->next;
		}

		struct nbs_instruments *instrument_node = instruments_head;
		int instrument_pitch = 45;
		for (int instrument_id = 0; instrument_node && instrument_node->next && instrument_id < notes->instrument; instrument_id++) {
			instrument_node = instrument_node->next;
			instrument_pitch = (int)instrument_node->pitch;
		}

		int instrument = (0 <= notes->instrument && notes->instrument <= 15)
							 ? notes->instrument : 0;
		float volume = ((float)notes->velocity / 100.0f) * ((float)layers_node->volume / 100.0f);
		float final_key = (float)notes->key + (float)(instrument_pitch - 45) + (float)((float)notes->pitch / 100.0f);
		float pitch = powf(2, (float)((final_key - 45) / 12.0f));

		int note_time = (int)((float)notes->tick * time_per_tick);

		struct note_queue_node *new_note_node = (struct note_queue_node *) malloc(sizeof(struct note_queue_node));
		if (!new_note_node)
			return NULL;

		new_note_node->time = note_time;
		new_note_node->instrument = instrument;
		new_note_node->volume = volume;
		new_note_node->pitch = pitch;
		new_note_node->next = NULL;

		if (note_node_head == NULL) {
			note_node_head = new_note_node;
			note_node_tail = new_note_node;
		} else {
			note_node_tail->next = new_note_node;
			note_node_tail = new_note_node;
		}

		notes = notes->next;
	}

	nbs_free_header(nbs_header);
	nbs_free_notes(notes_head);
	nbs_free_layers(layers_head);
	nbs_free_instruments(instruments_head);

	*total_time = note_node_tail->time;
	return note_node_head;
}

char music_player_save_to_file()
{
	xr_dynamic_array_info playlist_info;
	playlist_info.curr_arr_size = g_player_array_0_info.curr_arr_size + g_offline_player_array_0_info.curr_arr_size;
	playlist_info.singel_value_size = sizeof(struct player_music_info);
	playlist_info.start_addr = NULL;
	
	char path[4096];
	sprintf(path, "%s/playlist_save.bin", data_path);
	FILE *playlist_file =  fopen(path, "wb");
	fwrite(&playlist_info, sizeof(xr_dynamic_array_info), 1, playlist_file);
	fwrite(&g_player_array_0, sizeof(struct player_music_info), g_player_array_0_info.curr_arr_size, playlist_file);
	fwrite(&g_offline_player_array_0, sizeof(struct player_music_info), g_offline_player_array_0_info.curr_arr_size, playlist_file);
	fclose(playlist_file);
	return true;
}

long long find_player_in_array(struct player_music_info *in_array, unsigned long long in_array_size, struct player *in_player)
{
	for (unsigned int player_count = 0; player_count < in_array_size; player_count++) {
		if (in_array[player_count].player == in_player) return player_count;
	}
	return -1;
}

long long find_player_in_array_by_xuid(struct player_music_info *in_array, unsigned long long in_array_size, const char *in_xuid)
{
	
	unsigned long long cac_result = 0;
	const char *xuid_in_array = 0;
	const char *xuid_in_va = 0;

	for (unsigned int player_count = 0; player_count < in_array_size; player_count++) {
		cac_result = 0;
		xuid_in_va = in_xuid;
		xuid_in_array = in_array[player_count].player_xuid;
		while (!cac_result) cac_result = *xuid_in_array++ != *xuid_in_va++;
		if (!cac_result) return player_count;
	}
	return -1;
}

long long find_note_in_array(struct music_note_info *in_array, unsigned long long in_array_size, const char *in_music_name)
{
	unsigned long long cac_result = 0;
	const char *music_name_in_array = 0;
	const char *music_name_in_va = 0;

	for (unsigned int music_count = 0; music_count < in_array_size; music_count++) {
		cac_result = 0;
		music_name_in_va = in_music_name;
		music_name_in_array = in_array[music_count].song_name;
		while (!cac_result) cac_result = *music_name_in_array++ != *music_name_in_va++;
		if (!cac_result) return music_count;
	}
	return -1;
}

struct music_queue_node *get_player_last_music(struct player *in_player)
{
	long long player_pos_in_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, in_player);

	if (player_pos_in_array != -1) {
		struct music_queue_node *current_node = g_player_array_0[player_pos_in_array].music_queue_node;
		while(current_node->next) current_node = current_node->next;
		return current_node;
	} else return 0;
}



bool music_queue_add(struct player *player, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type)
{
	char *nbs_file_name_new = strdup(nbs_file_name);
	char nbs_path[4096];
	char v_song_name[256];
	strncpy(v_song_name, strtok(nbs_file_name_new, "."), 256);
	
	long long music_in_arr_pos = find_note_in_array(g_note_array_0, g_note_array_0_info.curr_arr_size, v_song_name);

	struct music_queue_node *node = (struct music_queue_node *) malloc(sizeof(struct music_queue_node));
	
	if (node == NULL) {
		server_logger(LOG_LEVEL_ERR, "Failed to allocate memory for new node.");
		return false;
	}
	// play_with_video(player, nbs_file_name, loop);


	if (music_in_arr_pos == -1) {
		long long curr_music_in_arr_pos = g_note_array_0_info.curr_arr_size;
		xr_operator_dynamic_array(&g_note_array_0_info, &g_note_array_0, g_note_array_0_info.curr_arr_size, XR_ARRAY_ADD);

		sprintf(nbs_path, "%s/%s", data_path_nbs, nbs_file_name);
		FILE *fp = fopen(nbs_path, "rb");
		if (!fp)
			return false;
		struct note_queue_node *node_head = generate_note_queue(fp, &g_note_array_0[curr_music_in_arr_pos].time);
		fclose(fp);


		g_note_array_0[curr_music_in_arr_pos].note_queue_ptr = node_head;
		strcpy(g_note_array_0[curr_music_in_arr_pos].song_name, v_song_name);

		node->note_queue_node = g_note_array_0[curr_music_in_arr_pos].note_queue_ptr;
		node->total_time = g_note_array_0[curr_music_in_arr_pos].time;
		node->note = &g_note_array_0[curr_music_in_arr_pos];
	} else {
		node->note_queue_node = g_note_array_0[music_in_arr_pos].note_queue_ptr;
		node->total_time = g_note_array_0[music_in_arr_pos].time;
		node->note = &g_note_array_0[music_in_arr_pos];
	}
	
	node->player = player;
	node->note_queue_node_start = node->note_queue_node;
	node->start_time = uv_hrtime();
	node->loop = loop;
	node->music_bar_type = music_bar_type;
	// strncpy(node->song_name, strtok(nbs_file_name_new, "."), sizeof(node->song_name));
	strcpy(node->song_name, v_song_name);
	node->prev = NULL;
	node->next = NULL;

	long long player_pos_in_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, player);

	if (player_pos_in_array == -1) {
		long long curr_player_pos_in_array = g_player_array_0_info.curr_arr_size;
		xr_operator_dynamic_array(&g_player_array_0_info, &g_player_array_0, g_player_array_0_info.curr_arr_size, XR_ARRAY_ADD);
		g_player_array_0[curr_player_pos_in_array].player = player;
		g_player_array_0[curr_player_pos_in_array].player_xuid = get_player_xuid(player);
		g_player_array_0[curr_player_pos_in_array].music_queue_node = node;
		g_player_array_0[curr_player_pos_in_array].paused = 0;
		g_player_array_0[curr_player_pos_in_array].music_num = 1;
	} else {
		struct music_queue_node *music_queue_last = get_player_last_music(player);
		node->prev = music_queue_last;
		music_queue_last->next = node;
		g_player_array_0[player_pos_in_array].music_num++;
	}

	//free useless ptr
	free(nbs_file_name_new);

	return true;
}

bool music_queue_del(struct player *player, unsigned long long in_pos)
{
	unsigned long long player_pos_in_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, player);
	struct music_queue_node *current_node = current_node = g_player_array_0[player_pos_in_array].music_queue_node;

	if (!in_pos) {
		if (current_node->next) {
			struct music_queue_node *temp_current_node = current_node;
			current_node = current_node->next;
			current_node->start_time = uv_hrtime();
			g_player_array_0[player_pos_in_array].music_queue_node = current_node;
			free(temp_current_node);
		} else {
			xr_operator_dynamic_array(&g_player_array_0_info, &g_player_array_0, player_pos_in_array, XR_ARRAY_DEL);
			send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
		}
	} else {
		while(in_pos && current_node->next) {
			current_node = current_node->next;
			in_pos--;
		}
		if (!in_pos) {
			if (current_node->next) {
				current_node->prev->next = current_node->next;
			} else {
				current_node->prev->next = NULL;
			}
			free(current_node);
		} else {
			return false;
		}
	}

	// player_pos_in_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, player);
	// if (player_pos_in_array != -1) g_player_array_0[player_pos_in_array].music_num--;

	if (g_player_array_0[player_pos_in_array].player == player)
		g_player_array_0[player_pos_in_array].music_num--; // I think it will cause error.

	return true;
}

void music_queue_del_all(struct player *player)
{
	long long player_pos_in_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, player);

	while(find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, player) != -1) {
		music_queue_del(player, g_player_array_0[player_pos_in_array].music_num - 1);
	}
	send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
}



void music_player_player_offline(struct player *in_player)
{
	long long player_pos_in_online_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, in_player);
	long long player_pos_in_offline_array = find_player_in_array(g_offline_player_array_0, g_offline_player_array_0_info.curr_arr_size, in_player);

	if(player_pos_in_offline_array == -1 && player_pos_in_online_array != -1) {
		xr_operator_dynamic_array(&g_offline_player_array_0_info, &g_offline_player_array_0, g_offline_player_array_0_info.curr_arr_size, XR_ARRAY_ADD);
		char *online_pos = (char *)&g_player_array_0[player_pos_in_online_array];
		char *offline_pos = (char *)&g_offline_player_array_0[g_offline_player_array_0_info.curr_arr_size - 1];
		for(int mem_pos = 0; mem_pos < sizeof(struct player_music_info); mem_pos++)
			*offline_pos++ = *online_pos++;
		xr_operator_dynamic_array(&g_player_array_0_info, &g_player_array_0, player_pos_in_online_array, XR_ARRAY_DEL);
	}
}

void music_player_player_online(struct player *in_player)
{
	const char *player_xuid = get_player_xuid(in_player);
	long long player_pos_in_online_array = find_player_in_array_by_xuid(g_player_array_0, g_player_array_0_info.curr_arr_size, player_xuid);
	long long player_pos_in_offline_array = find_player_in_array_by_xuid(g_offline_player_array_0, g_offline_player_array_0_info.curr_arr_size, player_xuid);

	if(player_pos_in_online_array == -1 && player_pos_in_offline_array != -1) {
		xr_operator_dynamic_array(&g_player_array_0_info, &g_player_array_0, g_player_array_0_info.curr_arr_size, XR_ARRAY_ADD);
		char *online_pos = (char *)&g_player_array_0[g_player_array_0_info.curr_arr_size - 1];
		char *offline_pos = (char *)&g_offline_player_array_0[player_pos_in_offline_array];
		for(int mem_pos = 0; mem_pos < sizeof(struct player_music_info); mem_pos++)
			*online_pos++ = *offline_pos++;
		g_player_array_0[g_player_array_0_info.curr_arr_size - 1].player = in_player;
		g_player_array_0[g_player_array_0_info.curr_arr_size - 1].music_queue_node->player = in_player;
		g_player_array_0[g_player_array_0_info.curr_arr_size - 1].music_queue_node->start_time = uv_hrtime() - g_player_array_0[g_player_array_0_info.curr_arr_size - 1].music_queue_node->note_queue_node->time * UV_HRT_PER_MS;
		xr_operator_dynamic_array(&g_offline_player_array_0_info, &g_offline_player_array_0, player_pos_in_offline_array, XR_ARRAY_DEL);
	}
	free((char *)player_xuid);
}



void music_player_query_music_queue(struct player *player)
{
	long long player_pos_in_array = find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, player);
	if (player_pos_in_array != -1) {
		int music_num = 1;
		char msg_to_player[512];
		struct music_queue_node *temp_music_node = g_player_array_0[player_pos_in_array].music_queue_node;

		send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] [Index] [Music Name] ---Playlist---\n");
		sprintf(msg_to_player, "§6[MediaPlayer] [%d] §a%s§6 (Current)\n", 0, temp_music_node->song_name);
		send_text_packet(player, TEXT_TYPE_RAW, msg_to_player);
		for (; temp_music_node->next; music_num++) {
			temp_music_node = temp_music_node->next;
			sprintf(msg_to_player, "§6[MediaPlayer] [%d] §a%s§6\n", music_num, temp_music_node->song_name);
			send_text_packet(player, TEXT_TYPE_RAW, msg_to_player);
		}
	} else {
		send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Playlist Empty!\n");
	}
}



void send_music_sound_packet(void)
{
	struct vec3 *player_pos = 0;
	struct player *cur_player = 0;
	const char *sound_name = 0;
	time_t current_time = 0;

	struct music_queue_node *node = 0;

	for (unsigned long long player_pos_in_array = 0; player_pos_in_array < g_player_array_0_info.curr_arr_size; player_pos_in_array++) {
		if (g_player_array_0[player_pos_in_array].paused) continue;
		node = g_player_array_0[player_pos_in_array].music_queue_node;
		cur_player = node->player;
		player_pos = actor_get_pos((struct actor *)node->player);
		struct note_queue_node *note_node;
		for (note_node = node->note_queue_node;
			note_node && note_node->time < ((long long)(uv_hrtime() - node->start_time)) / UV_HRT_PER_MS;
			note_node = note_node->next) {
			sound_name = BUILTIN_INSTRUMENT[note_node->instrument];
			current_time = note_node->time;

			send_play_sound_packet(node->player, sound_name, player_pos, note_node->volume, note_node->pitch);
		}
		if (current_time)
			set_music_bar(node->player, node);

		if (note_node) {
			node->note_queue_node = note_node;
		} else {
			if (node->loop > 1) {
				--node->loop;
				node->note_queue_node =
					node->note_queue_node_start;
				node->start_time = uv_hrtime() + 3000;
			} else {
				music_queue_del(node->player, 0);
				if (find_player_in_array(g_player_array_0, g_player_array_0_info.curr_arr_size, cur_player) != -1)
					g_player_array_0[player_pos_in_array].music_queue_node->start_time = uv_hrtime() + 1000000000;
			}
		}
	}
}

void set_music_bar(struct player *player, struct music_queue_node *node)
{
	int total_time_min = (int)(node->total_time / 1000 / 60);
	int total_time_sec = ((int)(node->total_time / 1000)) % 60;
	int passed_time_min = (int)(node->note_queue_node->time / 1000 / 60);
	int passed_time_sec = ((int)(node->note_queue_node->time / 1000)) % 60;
	float passed_rate = (float)node->note_queue_node->time / (float)node->total_time;
	char msg[128];
	sprintf(msg, "§e %.64s §7| §a %02d:%02d/%02d:%02d",
			node->song_name,
			passed_time_min,
			passed_time_sec,
			total_time_min,
			total_time_sec);

	switch (node->music_bar_type) {
	case MUSIC_BAR_TYPE_NOT_DISPLAY:
		break;
	case MUSIC_BAR_TYPE_BOSS_BAR:
		send_boss_event_packet(player, msg, passed_rate, BOSS_BAR_HIDE);
		send_boss_event_packet(player, msg, passed_rate, BOSS_BAR_DISPLAY);
		break;
	case MUSIC_BAR_TYPE_ACTION_BAR_JUKEBOX_POPUP:
		send_text_packet(player, TEXT_TYPE_JUKEBOX_POPUP, msg);
		break;
	case MUSIC_BAR_TYPE_ACTION_BAR_POPUP:
		send_text_packet(player, TEXT_TYPE_POPUP, msg);
		break;
	case MUSIC_BAR_TYPE_ACTION_BAR_TIP:
		send_text_packet(player, TEXT_TYPE_TIP, msg);
		break;
	default:
		break;
	}
}

void free_note_queue(struct note_queue_node *head)
{
	struct note_queue_node *current = head;
	struct note_queue_node *next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	head = NULL;
}

// void free_music_queue(void)
// {
// 	struct music_queue_node *current = music_queue_head;
// 	struct music_queue_node *next;
// 	while (current != NULL) {
// 		next = current->next;
// 		free(current);
// 		current = next;
// 	}
// 	music_queue_head = NULL;
// }

// struct music_queue_node *music_queue_get_player(struct player *player)
// {
// 	struct music_queue_node *current = music_queue_head;
// 	while (current != NULL) {
// 		if (current->player == player)
// 			return current;
// 		current = current->next;
// 	}
// 	return NULL;
// }

bool play_with_video(struct player *player, const char *filename, int loop)
{
    int folder_count;
    char **foldernames = get_foldernames(data_path_video, &folder_count);
    for (int i = 0; i < folder_count; i++) {
        if (strstr(filename, foldernames[i])) {
            char video_path[4096];
            sprintf(video_path, "%s/%s", data_path_video, foldernames[i]);
			video_queue_delete_player(player);
            video_queue_add_player(player, video_path, loop);
			return true;
        }
    }
	return false;
}
