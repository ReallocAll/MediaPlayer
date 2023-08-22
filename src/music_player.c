#include <mediaplayer/music_player.h>
#include <nbsparser/nbsparser.h>


struct music_queue_node *music_queue_head = NULL;

struct note_queue_node *generate_note_queue(FILE *fp, time_t *total_time)
{
	struct note_queue_node *note_queue_node_head = NULL;
	struct note_queue_node *note_queue_node_tail = NULL;

	struct nbs_header *nbs_header = nbs_parse_header(fp);
	struct nbs_notes *notes_head = nbs_parse_notes(fp, nbs_header->version);
	struct nbs_layers *layers_head = nbs_parse_layers(fp, nbs_header->song_layers, nbs_header->version);
	struct nbs_instruments *instruments_head = nbs_parse_instruments(fp, nbs_header->song_layers, nbs_header->version);

	struct nbs_notes *notes_curr = notes_head;
	float time_per_tick = (float)(20.0f / nbs_header->tempo * 50.0f);
	while (notes_curr != NULL) {

		struct nbs_layers *layers_curr = layers_head;
		for (int layer_count = 0; layer_count < notes_curr->layer; layer_count++) {
			layers_curr = layers_curr->next;
		}

		struct nbs_instruments *instrument_curr = instruments_head;
		int instrument_pitch = 45;
		for (int instrument_id = 0; instrument_curr && instrument_curr->next && instrument_id < notes_curr->instrument; instrument_id++) {
			instrument_curr = instrument_curr->next;
			instrument_pitch = (int)instrument_curr->pitch;
		}

		int instrument = (0 <= notes_curr->instrument && notes_curr->instrument <= 15)
							 ? notes_curr->instrument : 0;
		float volume = ((float)notes_curr->velocity / 100.0f) * ((float)layers_curr->volume / 100.0f);
		float final_key = (float)notes_curr->key + (float)(instrument_pitch - 45) + (float)((float)notes_curr->pitch / 100.0f);
		float pitch = powf(2, (float)((final_key - 45) / 12.0f));

		int note_time = (int)((float)notes_curr->tick * time_per_tick);

		struct note_queue_node *new_note_node = (struct note_queue_node *) malloc(sizeof(struct note_queue_node));
		if (!new_note_node)
			return (struct note_queue_node *)NULL;

		new_note_node->time = note_time;
		new_note_node->instrument = instrument;
		new_note_node->volume = volume;
		new_note_node->pitch = pitch;
		new_note_node->next = NULL;

		if (note_queue_node_head == NULL) {
			note_queue_node_head = new_note_node;
			note_queue_node_tail = new_note_node;
		} else {
			note_queue_node_tail->next = new_note_node;
			note_queue_node_tail = new_note_node;
		}

		notes_curr = notes_curr->next;
	}

	nbs_free_header(nbs_header);
	nbs_free_notes(notes_head);
	nbs_free_layers(layers_head);
	nbs_free_instruments(instruments_head);

	*total_time = note_queue_node_tail->time;
	return note_queue_node_head;
}

bool music_queue_add_player(long long xuid, const char *nbs_file_name, int loop)
{
	music_queue_delete_player(xuid);
	char *nbs_file_name_new = _strdup(nbs_file_name);
	char nbs_path[260];
	sprintf(nbs_path, "%s\\%s", data_path_nbs, nbs_file_name);
	FILE *fp = fopen(nbs_path, "rb");
	if (!fp)
		return false;
	time_t total_time = 0;
	struct note_queue_node *note_queue_head = generate_note_queue(fp, &total_time);
	fclose(fp);
	struct music_queue_node *new_node = (struct music_queue_node *) malloc(sizeof(struct music_queue_node));
	if (new_node == NULL) {
		server_logger("Failed to allocate memory for new node.", ERR);
		return false;
	}
	play_with_video(xuid, nbs_file_name, loop);

	new_node->xuid = xuid;
	new_node->note_queue_node = note_queue_head;
	new_node->note_queue_node_start = note_queue_head;
	new_node->start_time = clock();
	new_node->total_time = total_time;
	new_node->loop = loop;
	strncpy(new_node->song_name, strtok(nbs_file_name_new, "."), sizeof(new_node->song_name));
	new_node->next = NULL;

	if (music_queue_head == NULL) {
		music_queue_head = new_node;
		return true;
	}

	struct music_queue_node *curr_node = music_queue_head;
	while (curr_node->next != NULL) {
		curr_node = curr_node->next;
	}
	curr_node->next = new_node;
	free(nbs_file_name_new);
	return true;
}

void music_queue_delete_player(long long xuid)
{
	struct music_queue_node *curr_node = music_queue_head;
	struct music_queue_node *prev_node = NULL;

	while (curr_node != NULL) {
		if (curr_node->xuid == xuid) {
			free_note_queue(curr_node->note_queue_node_start);
			if (prev_node != NULL)
				prev_node->next = curr_node->next;
			else
				music_queue_head = curr_node->next;

			free(curr_node);
			char player_xuid[PLAYER_XUID_STR_LEN];
			_i64toa(xuid, player_xuid, 10);
			struct player *player = get_player_by_xuid(g_level, player_xuid);
			if (is_player(player))
				send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
			video_queue_delete_player(xuid);
			return;
		}
		prev_node = curr_node;
		curr_node = curr_node->next;
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

	struct music_queue_node *music_queue_node_curr = music_queue_head;
	while (music_queue_node_curr != NULL) {
		sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", music_queue_node_curr->xuid);
		player = get_player_by_xuid(g_level, player_xuid);
		if (!player) {
			struct music_queue_node *to_delete = music_queue_node_curr;
			music_queue_node_curr = music_queue_node_curr->next;
			music_queue_delete_player(to_delete->xuid);
			continue;
		}
		player_pos = actor_get_pos((struct actor *)player);

		struct note_queue_node *note_node;
		for (note_node = music_queue_node_curr->note_queue_node;
			note_node && (time_t)note_node->time + music_queue_node_curr->start_time < clock();
			note_node = note_node->next) {

			sound_name = BUILTIN_INSTRUMENT[note_node->instrument];
			current_time = note_node->time;

			send_play_sound_packet(player, sound_name, player_pos, note_node->volume, note_node->pitch);
		}
		if (current_time)
			set_music_boss_bar(player, music_queue_node_curr->total_time,
								current_time, music_queue_node_curr->song_name);

		if (note_node) {
			music_queue_node_curr->note_queue_node = note_node;
		} else {
			if (music_queue_node_curr->loop > 1) {
				--music_queue_node_curr->loop;
				music_queue_node_curr->note_queue_node =
					music_queue_node_curr->note_queue_node_start;
				music_queue_node_curr->start_time = clock();
				music_queue_node_curr->start_time += 3000; // interval
			} else {
				music_queue_delete_player(music_queue_node_curr->xuid);
			}
		}

		music_queue_node_curr = music_queue_node_curr->next;
	}
}

void set_music_boss_bar(struct player *player, time_t total_time, time_t current_time, const char *song_name)
{
	int total_time_min = (int)(total_time / 1000 / 60);
	int total_time_sec = ((int)(total_time / 1000)) % 60;
	int passed_time_min = (int)(current_time / 1000 / 60);
	int passed_time_sec = ((int)(current_time / 1000)) % 60;
	float passed_rate = (float)current_time / (float)total_time;
	char msg[128];
	sprintf(msg, "§bMediaPlayer §7| §e%s §7| §a%02d:%02d/%02d:%02d",
			song_name,
			passed_time_min,
			passed_time_sec,
			total_time_min,
			total_time_sec);
	send_boss_event_packet(player, msg, passed_rate, BOSS_BAR_HIDE);
	send_boss_event_packet(player, msg, passed_rate, BOSS_BAR_DISPLAY);
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
            video_queue_add_player(player_xuid, video_path, loop);
			return true;
        }
    }
	return false;
}
