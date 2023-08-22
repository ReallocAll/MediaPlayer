#include <mediaplayer/video_player.h>


struct video_queue *video_queue_array = NULL;
int video_queue_size = 0;

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

    video_queue_array = (struct video_queue *) 
			realloc(video_queue_array, (video_queue_size + 1) * sizeof(struct video_queue));
    video_queue_array[video_queue_size++] = *new_node;

    free(new_node);

	return true;
}

void video_queue_delete_player(long long xuid)
{
	for (int i = 0; i < video_queue_size; i++) {
		if (video_queue_array[i].xuid == xuid) {
			for (int j = i; j < video_queue_size - 1; j++) {
				video_queue_array[j] = video_queue_array[j + 1];
			}
			video_queue_array = (struct video_queue *)
					realloc(video_queue_array, (video_queue_size - 1) * sizeof(struct video_queue));
			video_queue_size--;
			return;
		}
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
			video_queue_node->current_frame = 1;
			video_queue_node->start_time = clock();
		} else {
			video_queue_delete_player(video_queue_node->xuid);
		}
        return;
    }
    fclose(fp);

    struct rgb **pixels = get_png_pixels(filepath);
    static void *set_pixel_func = NULL;
    if (!set_pixel_func)
        set_pixel_func = dlsym("?setPixel@MapItemSavedData@@QEAA_NIII@Z");
    for (int y = 0; y < 128; ++y) {
        for (int x = 0; x < 128; ++x) {
            VIRTUAL_CALL(set_pixel_func,
                char (*)(struct map_item_saved_data *, int pixel, unsigned int row, unsigned int line),
                map_data, 0xFF000000 | *(unsigned int *)&pixels[y][x], x, y);
        }
		free(pixels[y]);
    }
	free(pixels);
    TLCALL("?setLocked@MapItemSavedData@@QEAAXXZ",
        void (*)(struct map_item_saved_data *),
        map_data);

    video_queue_node->current_frame = frame_index;
}

void free_video_queue(void)
{
    free(video_queue_array);
    video_queue_array = NULL;
    video_queue_size = 0;
}

struct video_queue *video_queue_get_player(long long player_xuid)
{
    for (int i = 0; i < video_queue_size; i++) {
        if (video_queue_array[i].xuid == player_xuid)
            return &video_queue_array[i];
    }
    return NULL;
}
