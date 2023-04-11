#include <mediaplayer/video_player.h>

struct video_queue *video_queue_head = NULL;

bool video_queue_add_player(long long xuid, char *video_path)
{
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
	char player_xuid[PLAYER_XUID_STR_LEN];
    sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", video_queue_node->xuid);
    struct player *player = get_player_by_xuid(g_level, player_xuid);
    char filepath[260];
    sprintf(filepath, "%s\\%04d.bin", video_queue_node->video_path, video_queue_node->current_frame);
    FILE *fp = fopen(filepath, "rb");

    if (!fp) {
        video_queue_delete_player(video_queue_node->xuid);
        return;
    }

    unsigned int val;
    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 128; ++j) {
            fread(&val, sizeof(val), 1, fp);
            TLCALL("?setPixel@MapItemSavedData@@QEAA_NIII@Z",
                char (*)(struct map_item_saved_data *, int pixel, unsigned int row, unsigned int line),
                map_data, 0xff000000 | val, j, i);
        }
    }
    TLCALL("?setLocked@MapItemSavedData@@QEAAXXZ",
        void (*)(struct map_item_saved_data *),
        map_data);

    fclose(fp);
    video_queue_node->current_frame++;
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
