#include <mediaplayer/music_player.h>
#include <nbsparser/nbsparser.h>


struct music_queue_node *music_queue_head = NULL;

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

bool music_queue_add_player(long long xuid, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type)
{
	music_queue_delete_player(xuid);
	char *nbs_file_name_new = _strdup(nbs_file_name);
	char nbs_path[260];
	sprintf(nbs_path, "%s\\%s", data_path_nbs, nbs_file_name);
	FILE *fp = fopen(nbs_path, "rb");
	if (!fp)
		return false;
	time_t total_time = 0;
	struct note_queue_node *node_head = generate_note_queue(fp, &total_time);
	fclose(fp);
	struct music_queue_node *node = (struct music_queue_node *) malloc(sizeof(struct music_queue_node));
	if (node == NULL) {
		server_logger("Failed to allocate memory for new node.", ERR);
		return false;
	}
	play_with_video(xuid, nbs_file_name, loop);

	node->xuid = xuid;
	node->note_queue_node = node_head;
	node->note_queue_node_start = node_head;
	node->start_time = clock();
	node->total_time = total_time;
	node->loop = loop;
	node->music_bar_type = music_bar_type;
	strncpy(node->song_name, strtok(nbs_file_name_new, "."), sizeof(node->song_name));
	node->next = NULL;

	if (music_queue_head == NULL) {
		music_queue_head = node;
		return true;
	}

	struct music_queue_node *curr_node = music_queue_head;
	while (curr_node->next != NULL) {
		curr_node = curr_node->next;
	}
	curr_node->next = node;
	free(nbs_file_name_new);
	return true;
}

void music_queue_delete_player(long long xuid)
{
	struct music_queue_node *node = music_queue_head;
	struct music_queue_node *node_prev = NULL;

	while (node != NULL) {
		if (node->xuid == xuid) {
			free_note_queue(node->note_queue_node_start);
			if (node_prev != NULL)
				node_prev->next = node->next;
			else
				music_queue_head = node->next;

			free(node);
			char player_xuid[PLAYER_XUID_STR_LEN];
			_i64toa(xuid, player_xuid, 10);
			struct player *player = get_player_by_xuid(player_xuid);
			if (is_player(player))
				send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
			video_queue_delete_player(xuid);
			return;
		}
		node_prev = node;
		node = node->next;
	}
}

void send_music_sound_packet(void)
{
	struct player *player;
	const char *player_name;
	struct vec3 *player_pos;
	char player_xuid[PLAYER_XUID_STR_LEN];
	const char *sound_name;
	time_t total_time;
	time_t current_time = 0;

	struct music_queue_node *node = music_queue_head;
	while (node != NULL) {
		sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", node->xuid);
		player = get_player_by_xuid(player_xuid);
		if (!player) {
			struct music_queue_node *to_delete = node;
			node = node->next;
			music_queue_delete_player(to_delete->xuid);
			continue;
		}
		player_pos = actor_get_pos((struct actor *)player);

		struct note_queue_node *note_node;
		for (note_node = node->note_queue_node;
			note_node && (time_t)note_node->time + node->start_time < clock();
			note_node = note_node->next) {

			sound_name = BUILTIN_INSTRUMENT[note_node->instrument];
			current_time = note_node->time;

			send_play_sound_packet(player, sound_name, player_pos, note_node->volume, note_node->pitch);
		}
		if (current_time)
			set_music_bar(player, node);

		if (note_node) {
			node->note_queue_node = note_node;
		} else {
			if (node->loop > 1) {
				--node->loop;
				node->note_queue_node =
					node->note_queue_node_start;
				node->start_time = clock();
				node->start_time += 3000; // interval
			} else {
				music_queue_delete_player(node->xuid);
			}
		}

		node = node->next;
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
	sprintf(msg, "§e♫ %s §7| §a√ %02d:%02d/%02d:%02d",
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

void free_music_queue(void)
{
	struct music_queue_node *current = music_queue_head;
	struct music_queue_node *next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	music_queue_head = NULL;
}


struct music_queue_node *music_queue_get_player(long long player_xuid)
{
	struct music_queue_node *current = music_queue_head;
	while (current != NULL) {
		if (current->xuid == player_xuid)
			return current;
		current = current->next;
	}
	return NULL;
}

bool play_with_video(long long player_xuid, const char *filename, int loop)
{
    int folder_count;
    const char **foldernames = get_foldernames(data_path_video, &folder_count);
    for (int i = 0; i < folder_count; i++) {
        if (strstr(filename, foldernames[i])) {
            char video_path[260];
            sprintf(video_path, "%s\\%s", data_path_video, foldernames[i]);
			video_queue_delete_player(player_xuid);
            video_queue_add_player(player_xuid, video_path, loop);
			return true;
        }
    }
	return false;
}
