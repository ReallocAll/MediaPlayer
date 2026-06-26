#include <mediaplayer/music_player.h>
#include <stb/stb_ds.h>

extern char data_path[4096];

static song_cache_entry_t *g_song_cache = NULL;
player_music_t *g_player_arr = NULL;
player_music_t *g_offline_player_arr = NULL;

/* ---- Song cache ---- */

song_cache_entry_t *song_cache_parse(FILE *fp, const char *song_name)
{
	struct nbs_header *nbs_header = nbs_parse_header(fp);
	if (!nbs_header)
		return NULL;

	struct nbs_notes *notes_head = nbs_parse_notes(fp, nbs_header->version);
	struct nbs_layers *layers_head = nbs_parse_layers(fp, nbs_header->song_layers, nbs_header->version);
	struct nbs_instruments *instruments_head = nbs_parse_instruments(fp, nbs_header->song_layers, nbs_header->version);

	song_cache_entry_t entry;
	memset(&entry, 0, sizeof(entry));
	strncpy(entry.song_name, song_name, sizeof(entry.song_name) - 1);
	entry.notes = NULL;

	float time_per_tick = (float)(20.0f / nbs_header->tempo * 50.0f);
	struct nbs_notes *notes = notes_head;

	while (notes != NULL) {
		struct nbs_layers *layers_node = layers_head;
		for (int layer_count = 0; layer_count < notes->layer; layer_count++) {
			if (layers_node->next == NULL)
				break;
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

		note_t nt;
		nt.time = (long long)((float)notes->tick * time_per_tick);
		nt.instrument = instrument;
		nt.volume = volume;
		nt.pitch = pitch;
		arrput(entry.notes, nt);

		notes = notes->next;
	}

	nbs_free_header(nbs_header);
	nbs_free_notes(notes_head);
	nbs_free_layers(layers_head);
	nbs_free_instruments(instruments_head);

	if (arrlen(entry.notes) > 0)
		entry.duration_ms = entry.notes[arrlen(entry.notes) - 1].time;
	else
		entry.duration_ms = 0;

	arrput(g_song_cache, entry);
	return &g_song_cache[arrlen(g_song_cache) - 1];
}

static long long song_cache_find(const char *song_name)
{
	int len = (int)arrlen(g_song_cache);
	for (int i = 0; i < len; i++) {
		if (strcmp(g_song_cache[i].song_name, song_name) == 0)
			return i;
	}
	return -1;
}

/* ---- Player lookup ---- */

long long player_music_find(player_music_t *arr, struct player *in_player)
{
	int len = (int)arrlen(arr);
	for (int i = 0; i < len; i++) {
		if (arr[i].player == in_player) return i;
	}
	return -1;
}

long long player_music_find_by_xuid(player_music_t *arr, const char *in_xuid)
{
	int len = (int)arrlen(arr);
	for (int i = 0; i < len; i++) {
		if (strcmp(arr[i].player_xuid, in_xuid) == 0)
			return i;
	}
	return -1;
}

/* ---- Playlist operations ---- */

bool player_music_enqueue(struct player *player, const char *nbs_file_name, int loop, enum music_bar_type music_bar_type)
{
	char *nbs_file_name_new = strdup(nbs_file_name);
	char nbs_path[4096];
	char v_song_name[256];

	if (!nbs_file_name_new) {
		server_logger(LOG_LEVEL_ERR, "Failed to allocate memory for filename.");
		return false;
	}

	strncpy(v_song_name, strtok(nbs_file_name_new, "."), sizeof(v_song_name) - 1);
	v_song_name[sizeof(v_song_name) - 1] = '\0';

	long long cache_idx = song_cache_find(v_song_name);
	song_cache_entry_t *song;

	if (cache_idx == -1) {
		sprintf(nbs_path, "%s/%s", data_path_nbs, nbs_file_name);
		FILE *fp = fopen(nbs_path, "rb");
		if (!fp) {
			free(nbs_file_name_new);
			return false;
		}
		song = song_cache_parse(fp, v_song_name);
		fclose(fp);
		if (!song) {
			free(nbs_file_name_new);
			return false;
		}
	} else {
		song = &g_song_cache[cache_idx];
	}

	music_queue_entry_t entry;
	entry.song = song;
	entry.cursor = 0;
	entry.start_time = uv_hrtime();
	entry.loop = loop;
	entry.bar_type = music_bar_type;

	long long player_pos = player_music_find(g_player_arr, player);

	if (player_pos == -1) {
		player_music_t pm;
		memset(&pm, 0, sizeof(pm));
		pm.player = player;
		pm.player_xuid = (char *)get_player_xuid(player);
		pm.playlist = NULL;
		pm.paused = false;
		arrput(pm.playlist, entry);
		pm.current_track = 0;
		arrput(g_player_arr, pm);
	} else {
		player_music_t *pm = &g_player_arr[player_pos];
		arrput(pm->playlist, entry);
	}

	free(nbs_file_name_new);
	return true;
}

bool player_music_dequeue(struct player *player, size_t index)
{
	long long pos = player_music_find(g_player_arr, player);
	if (pos < 0)
		return false;

	player_music_t *pm = &g_player_arr[pos];

	if (index >= arrlen(pm->playlist))
		return false;

	if (index == pm->current_track) {
		arrdel(pm->playlist, (int)index);
		if (arrlen(pm->playlist) == 0) {
			free(pm->player_xuid);
			arrfree(pm->playlist);
			arrdelswap(g_player_arr, (int)pos);
			send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
		} else {
			if (pm->current_track >= arrlen(pm->playlist))
				pm->current_track = arrlen(pm->playlist) - 1;
			pm->playlist[pm->current_track].start_time = uv_hrtime();
			pm->playlist[pm->current_track].cursor = 0;
		}
	} else {
		arrdel(pm->playlist, (int)index);
		if (index < pm->current_track)
			pm->current_track--;
	}

	return true;
}

void player_music_stop(struct player *player)
{
	long long pos = player_music_find(g_player_arr, player);
	if (pos < 0)
		return;

	player_music_t *pm = &g_player_arr[pos];
	free(pm->player_xuid);
	arrfree(pm->playlist);
	arrdelswap(g_player_arr, (int)pos);
	send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
}

/* ---- Offline/online ---- */

void music_player_player_offline(struct player *in_player)
{
	long long player_pos_online = player_music_find(g_player_arr, in_player);
	long long player_pos_offline = player_music_find(g_offline_player_arr, in_player);

	if (player_pos_offline == -1 && player_pos_online != -1) {
		arrput(g_offline_player_arr, g_player_arr[player_pos_online]);
		arrdelswap(g_player_arr, (int)player_pos_online);
	}
}

void music_player_player_online(struct player *in_player)
{
	const char *player_xuid = get_player_xuid(in_player);
	long long player_pos_online = player_music_find_by_xuid(g_player_arr, player_xuid);
	long long player_pos_offline = player_music_find_by_xuid(g_offline_player_arr, player_xuid);

	if (player_pos_online == -1 && player_pos_offline != -1) {
		player_music_t pm = g_offline_player_arr[player_pos_offline];
		pm.player = in_player;
		if (arrlen(pm.playlist) > 0) {
			music_queue_entry_t *entry = &pm.playlist[pm.current_track];
			entry->start_time = uv_hrtime() - g_song_cache->notes[0].time * UV_HRT_PER_MS;
		}
		arrput(g_player_arr, pm);
		arrdelswap(g_offline_player_arr, (int)player_pos_offline);
	}
	free((char *)player_xuid);
}

/* ---- Query playlist ---- */

void music_player_query_music_queue(struct player *player)
{
	long long pos = player_music_find(g_player_arr, player);
	if (pos < 0) {
		send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Playlist Empty!\n");
		return;
	}

	player_music_t *pm = &g_player_arr[pos];
	int len = (int)arrlen(pm->playlist);
	char msg_to_player[512];

	send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] [Index] [Music Name] ---Playlist---\n");
	for (int i = 0; i < len; i++) {
		const char *marker = (i == (int)pm->current_track) ? " (Current)" : "";
		sprintf(msg_to_player, "§6[MediaPlayer] [%d] §a%s§6%s\n",
			i, pm->playlist[i].song->song_name, marker);
		send_text_packet(player, TEXT_TYPE_RAW, msg_to_player);
	}
}

/* ---- Tick: send sounds ---- */

void send_music_sound_packet(void)
{
	int len = (int)arrlen(g_player_arr);
	struct vec3 *player_pos;

	for (int i = 0; i < len; i++) {
		player_music_t *pm = &g_player_arr[i];
		if (pm->paused)
			continue;
		if (arrlen(pm->playlist) == 0)
			continue;

		music_queue_entry_t *entry = &pm->playlist[pm->current_track];
		song_cache_entry_t *song = entry->song;
		player_pos = actor_get_pos((struct actor *)pm->player);
		time_t elapsed = (time_t)((uv_hrtime() - entry->start_time) / UV_HRT_PER_MS);
		time_t current_time = 0;
		size_t notes_len = arrlen(song->notes);

		/* Play all notes that have passed */
		while (entry->cursor < notes_len && song->notes[entry->cursor].time < elapsed) {
			note_t *nt = &song->notes[entry->cursor];
			send_play_sound_packet(pm->player, BUILTIN_INSTRUMENT[nt->instrument],
				player_pos, nt->volume, nt->pitch);
			current_time = nt->time;
			entry->cursor++;
		}

		if (current_time)
			set_music_bar_entry(pm->player, entry);

		/* Check if song ended */
		if (entry->cursor >= notes_len) {
			if (entry->loop > 1) {
				entry->loop--;
				entry->cursor = 0;
				entry->start_time = uv_hrtime() + 3000;
			} else {
				player_music_dequeue(pm->player, pm->current_track);
				i--;
				len--;
				continue;
			}
		}
	}
}

/* ---- Music bar ---- */

void set_music_bar_entry(struct player *player, music_queue_entry_t *entry)
{
	song_cache_entry_t *song = entry->song;
	size_t n = entry->cursor;
	if (n >= arrlen(song->notes))
		return;

	time_t note_time = song->notes[n].time;
	time_t total = song->duration_ms;
	if (total == 0) total = 1;

	int total_min = (int)(total / 1000 / 60);
	int total_sec = (int)((total / 1000) % 60);
	int passed_min = (int)(note_time / 1000 / 60);
	int passed_sec = (int)((note_time / 1000) % 60);
	float passed_rate = (float)note_time / (float)total;

	char msg[128];
	sprintf(msg, "§e %.64s §7| §a %02d:%02d/%02d:%02d",
		song->song_name, passed_min, passed_sec, total_min, total_sec);

	switch (entry->bar_type) {
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

/* ---- Save to file ---- */

char music_player_save_to_file(void)
{
	FILE *playlist_file;
	char path[4096];
	size_t player_len = (size_t)arrlen(g_player_arr);
	size_t offline_len = (size_t)arrlen(g_offline_player_arr);

	sprintf(path, "%s/playlist_save.bin", data_path);
	playlist_file = fopen(path, "wb");
	if (!playlist_file)
		return false;

	fwrite(&player_len, sizeof(size_t), 1, playlist_file);
	fwrite(g_player_arr, sizeof(player_music_t), player_len, playlist_file);
	fwrite(&offline_len, sizeof(size_t), 1, playlist_file);
	fwrite(g_offline_player_arr, sizeof(player_music_t), offline_len, playlist_file);
	fclose(playlist_file);
	return true;
}
