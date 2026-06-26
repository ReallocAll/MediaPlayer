#include <mediaplayer/music_player.h>
#include <stb/stb_ds.h>

extern char data_path[4096];

struct music_player_ctx g_music_ctx;

// ---- Song cache ----

struct song_cache_entry *song_cache_parse(FILE *fp, const char *song_name)
{
	struct nbs_song *nbs = nbs_parse(fp);
	if (!nbs)
		return nullptr;

	struct song_cache_entry entry;
	memset(&entry, 0, sizeof(entry));
	strncpy(entry.song_name, song_name, sizeof(entry.song_name) - 1);
	entry.notes = nullptr;

	float time_per_tick = (float)(20.0f / nbs->tempo * 50.0f);
	int note_count = (int)arrlen(nbs->notes);
	int layer_count = (int)arrlen(nbs->layers);
	int instr_count = (int)arrlen(nbs->instruments);

	for (int i = 0; i < note_count; i++) {
		struct nbs_note *nn = &nbs->notes[i];

		// Find layer by index
		int layer_idx = nn->layer;
		struct nbs_layer *layer = (layer_idx < layer_count) ? &nbs->layers[layer_idx] : nullptr;

		// Find instrument pitch
		int instrument_pitch = 45;
		int instr_idx = nn->instrument;
		if (instr_idx < instr_count)
			instrument_pitch = (int)nbs->instruments[instr_idx].pitch;

		int instrument = (0 <= nn->instrument && nn->instrument <= 15)
				 ? nn->instrument : 0;
		float volume = ((float)nn->velocity / 100.0f);
		if (layer)
			volume *= ((float)layer->volume / 100.0f);
		float final_key = (float)nn->key + (float)(instrument_pitch - 45) + (float)((float)nn->pitch / 100.0f);
		float pitch = powf(2, (float)((final_key - 45) / 12.0f));

		struct note nt;
		nt.time = (long long)((float)nn->tick * time_per_tick);
		nt.instrument = instrument;
		nt.volume = volume;
		nt.pitch = pitch;
		arrput(entry.notes, nt);
	}

	nbs_free(nbs);

	if (arrlen(entry.notes) > 0)
		entry.duration_ms = entry.notes[arrlen(entry.notes) - 1].time;
	else
		entry.duration_ms = 0;

	arrput(g_music_ctx.song_cache, entry);
	return &g_music_ctx.song_cache[arrlen(g_music_ctx.song_cache) - 1];
}

static long long song_cache_find(const char *song_name)
{
	int len = (int)arrlen(g_music_ctx.song_cache);
	for (int i = 0; i < len; i++) {
		if (strcmp(g_music_ctx.song_cache[i].song_name, song_name) == 0)
			return i;
	}
	return -1;
}

// ---- Player lookup ----

long long player_music_find(struct player_music *arr, struct player *in_player)
{
	int len = (int)arrlen(arr);
	for (int i = 0; i < len; i++) {
		if (arr[i].player == in_player) return i;
	}
	return -1;
}

long long player_music_find_by_xuid(struct player_music *arr, const char *in_xuid)
{
	int len = (int)arrlen(arr);
	for (int i = 0; i < len; i++) {
		if (strcmp(arr[i].player_xuid, in_xuid) == 0)
			return i;
	}
	return -1;
}

// ---- Playlist operations ----

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
	struct song_cache_entry *song;

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
		song = &g_music_ctx.song_cache[cache_idx];
	}

	struct music_queue_entry entry;
	entry.song = song;
	entry.cursor = 0;
	entry.start_time = uv_hrtime();
	entry.loop = loop;
	entry.bar_type = music_bar_type;

	long long player_pos = player_music_find(g_music_ctx.online_players, player);

	if (player_pos == -1) {
		struct player_music pm;
		memset(&pm, 0, sizeof(pm));
		pm.player = player;
		pm.player_xuid = (char *)get_player_xuid(player);
		pm.playlist = nullptr;
		pm.paused = false;
		arrput(pm.playlist, entry);
		pm.current_track = 0;
		arrput(g_music_ctx.online_players, pm);
	} else {
		struct player_music *pm = &g_music_ctx.online_players[player_pos];
		arrput(pm->playlist, entry);
	}

	free(nbs_file_name_new);
	return true;
}

bool player_music_dequeue(struct player *player, size_t index)
{
	long long pos = player_music_find(g_music_ctx.online_players, player);
	if (pos < 0)
		return false;

	struct player_music *pm = &g_music_ctx.online_players[pos];

	if (index >= arrlen(pm->playlist))
		return false;

	if (index == pm->current_track) {
		arrdel(pm->playlist, (int)index);
		if (arrlen(pm->playlist) == 0) {
			free(pm->player_xuid);
			arrfree(pm->playlist);
			arrdelswap(g_music_ctx.online_players, (int)pos);
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
	long long pos = player_music_find(g_music_ctx.online_players, player);
	if (pos < 0)
		return;

	struct player_music *pm = &g_music_ctx.online_players[pos];
	free(pm->player_xuid);
	arrfree(pm->playlist);
	arrdelswap(g_music_ctx.online_players, (int)pos);
	send_boss_event_packet(player, "", 0, BOSS_BAR_HIDE);
}

// ---- Offline/online ----

void music_player_player_offline(struct player *in_player)
{
	long long player_pos_online = player_music_find(g_music_ctx.online_players, in_player);
	long long player_pos_offline = player_music_find(g_music_ctx.offline_players, in_player);

	if (player_pos_offline == -1 && player_pos_online != -1) {
		arrput(g_music_ctx.offline_players, g_music_ctx.online_players[player_pos_online]);
		arrdelswap(g_music_ctx.online_players, (int)player_pos_online);
	}
}

void music_player_player_online(struct player *in_player)
{
	const char *player_xuid = get_player_xuid(in_player);
	long long player_pos_online = player_music_find_by_xuid(g_music_ctx.online_players, player_xuid);
	long long player_pos_offline = player_music_find_by_xuid(g_music_ctx.offline_players, player_xuid);

	if (player_pos_online == -1 && player_pos_offline != -1) {
		struct player_music pm = g_music_ctx.offline_players[player_pos_offline];
		pm.player = in_player;
		if (arrlen(pm.playlist) > 0) {
			struct music_queue_entry *entry = &pm.playlist[pm.current_track];
			entry->start_time = uv_hrtime() - g_music_ctx.song_cache->notes[0].time * UV_HRT_PER_MS;
		}
		arrput(g_music_ctx.online_players, pm);
		arrdelswap(g_music_ctx.offline_players, (int)player_pos_offline);
	}
	free((char *)player_xuid);
}

// ---- Query playlist ----

void music_player_query_music_queue(struct player *player)
{
	long long pos = player_music_find(g_music_ctx.online_players, player);
	if (pos < 0) {
		send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Playlist Empty!\n");
		return;
	}

	struct player_music *pm = &g_music_ctx.online_players[pos];
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

// ---- Tick: send sounds ----

void send_music_sound_packet(void)
{
	int len = (int)arrlen(g_music_ctx.online_players);
	struct vec3 *player_pos;

	for (int i = 0; i < len; i++) {
		struct player_music *pm = &g_music_ctx.online_players[i];
		if (pm->paused)
			continue;
		if (arrlen(pm->playlist) == 0)
			continue;

		struct music_queue_entry *entry = &pm->playlist[pm->current_track];
		struct song_cache_entry *song = entry->song;
		player_pos = actor_get_pos((struct actor *)pm->player);
		time_t elapsed = (time_t)((uv_hrtime() - entry->start_time) / UV_HRT_PER_MS);
		time_t current_time = 0;
		size_t notes_len = arrlen(song->notes);

		// Play all notes that have passed
		while (entry->cursor < notes_len && song->notes[entry->cursor].time < elapsed) {
			struct note *nt = &song->notes[entry->cursor];
			send_play_sound_packet(pm->player, BUILTIN_INSTRUMENT[nt->instrument],
				player_pos, nt->volume, nt->pitch);
			current_time = nt->time;
			entry->cursor++;
		}

		if (current_time)
			set_music_bar_entry(pm->player, entry);

		// Check if song ended
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

// ---- Music bar ----

void set_music_bar_entry(struct player *player, struct music_queue_entry *entry)
{
	struct song_cache_entry *song = entry->song;
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

// ---- Save to file ----

char music_player_save_to_file(void)
{
	FILE *playlist_file;
	char path[4096];
	size_t player_len = (size_t)arrlen(g_music_ctx.online_players);
	size_t offline_len = (size_t)arrlen(g_music_ctx.offline_players);

	sprintf(path, "%s/playlist_save.bin", data_path);
	playlist_file = fopen(path, "wb");
	if (!playlist_file)
		return false;

	fwrite(&player_len, sizeof(size_t), 1, playlist_file);
	fwrite(g_music_ctx.online_players, sizeof(struct player_music), player_len, playlist_file);
	fwrite(&offline_len, sizeof(size_t), 1, playlist_file);
	fwrite(g_music_ctx.offline_players, sizeof(struct player_music), offline_len, playlist_file);
	fclose(playlist_file);
	return true;
}
