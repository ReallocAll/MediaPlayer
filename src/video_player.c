#include <mediaplayer/video_player.h>

struct video_queue *video_queue_head = NULL;

bool video_queue_add_player(long long xuid, char *video_path, int loop)
{
    video_queue_delete_player(xuid);
	struct video_queue *new_node = (struct video_queue *) malloc(sizeof(struct video_queue));
	if (new_node == NULL) {
		server_logger("Failed to allocate memory for new node.", ERR);
		return false;
	}
    int total_frames;
    char **filenames = get_filenames(video_path, &total_frames);
    free_filenames(filenames, total_frames);

	new_node->xuid = xuid;
	new_node->start_time = clock();
	strncpy(new_node->video_path, video_path, sizeof(new_node->video_path));
	new_node->total_frames = total_frames;
	new_node->current_frame = 1;
	new_node->loop = loop;
	new_node->next = NULL;

	if (video_queue_head == NULL) {
		video_queue_head = new_node;
		return true;
	}

	struct video_queue *curr_node = video_queue_head;
	while (curr_node->next != NULL) {
		curr_node = curr_node->next;
	}
	curr_node->next = new_node;
	return true;
}

void video_queue_delete_player(long long xuid)
{
	struct video_queue *curr_node = video_queue_head;
	struct video_queue *prev_node = NULL;

	while (curr_node != NULL) {
		if (curr_node->xuid == xuid) {
			if (prev_node != NULL)
				prev_node->next = curr_node->next;
			else
				video_queue_head = curr_node->next;

			free(curr_node);
			return;
		}
		prev_node = curr_node;
		curr_node = curr_node->next;
	}
}

void play_video(struct video_queue *video_queue_node, struct map_item_saved_data *map_data)
{
	struct music_queue_node *music_queue_node = music_queue_get_player(video_queue_node->xuid);
	if (music_queue_node && music_queue_node->note_queue_node == music_queue_node->note_queue_node_start)
		return;
	char player_xuid[PLAYER_XUID_STR_LEN];
    sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", video_queue_node->xuid);
    struct player *player = get_player_by_xuid(g_level, player_xuid);
    char filepath[260];

	if (((time_t)clock() - video_queue_node->start_time) <= 0)
		return;
    // 1000ms / 50 = 20FPS
    int frame_index = (int)(((time_t)clock() - video_queue_node->start_time) / 50) + 1;

    sprintf(filepath, "%s\\%09d.png", video_queue_node->video_path, frame_index);
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
		if (video_queue_node->loop > 1) {
			--video_queue_node->loop;
			// Solution 1
			video_queue_node->current_frame = 1;
			video_queue_node->start_time = clock();
			// video_queue_node->start_time += 3000; // interval
			// Solution 2
			// video_queue_add_player(video_queue_node->xuid,
			// 		video_queue_node->video_path,
			// 		video_queue_node->loop);
		} else {
			video_queue_delete_player(video_queue_node->xuid);
		}
        return;
    }
    fclose(fp);

    struct rgb **pixels = get_png_pixels(filepath);
    unsigned int val = 0;
    for (int y = 0; y < 128; ++y) {
        for (int x = 0; x < 128; ++x) {
            memcpy((unsigned char *)&val, &pixels[y][x], sizeof(pixels[y][x]));
            TLCALL("?setPixel@MapItemSavedData@@QEAA_NIII@Z",
                char (*)(struct map_item_saved_data *, int pixel, unsigned int row, unsigned int line),
                map_data, 0xff000000 | val, x, y);
        }
    }
    TLCALL("?setLocked@MapItemSavedData@@QEAAXXZ",
        void (*)(struct map_item_saved_data *),
        map_data);

	for (int y = 0; y < 128; y++) {
		free(pixels[y]);
	}
	free(pixels);

    video_queue_node->current_frame = frame_index;
}

void free_video_queue(void)
{
	struct video_queue *current = video_queue_head;
	struct video_queue *next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	video_queue_head = NULL;
}

struct video_queue *video_queue_get_player(long long player_xuid)
{
	struct video_queue *current = video_queue_head;
	while (current != NULL) {
		if (current->xuid == player_xuid)
			return current;
		current = current->next;
	}
	return NULL;
}
