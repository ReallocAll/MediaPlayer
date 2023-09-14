#include <mediaplayer/video_player.h>

struct video_queue *video_queue_array = NULL;
int video_queue_size = 0;

void CALLBACK get_video_frame(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work)
{
    struct video_queue *node = (struct video_queue *)parameter;
    FILE *fp = NULL;
    char filepath[260];
	char player_xuid[PLAYER_XUID_STR_LEN];
    sprintf(filepath, "%s\\%09d.png", node->video_path, 1);
    fp = fopen(filepath, "rb");
    get_pixels(fp, &node->ihdr, NULL, true);
    fclose(fp);
    node->image = malloc(node->ihdr.width * node->ihdr.height * 4);
    sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", node->xuid);
    while (node->image) {
		if (!get_player_by_xuid(player_xuid)) {
		    video_queue_delete_player(node->xuid);
            return;
        }

        // 1000ms / 50 = 20FPS
        int frame_index = (int)(((time_t)clock() - node->start_time) / 50) + 1;
        if (frame_index == node->current_frame) {
            Sleep(1);
            continue;
        }

        sprintf(filepath, "%s\\%09d.png", node->video_path, frame_index);
        fp = fopen(filepath, "rb");
        if (!fp) {
            if (node->loop > 1) {
                --node->loop;
                node->current_frame = 1;
                node->start_time = clock();
            } else {
                video_queue_delete_player(node->xuid);
                return;
            }
        }

        get_pixels(fp, &node->ihdr, node->image, false);
        node->current_frame = frame_index;
        fclose(fp);
    }
}

bool video_queue_add_player(long long xuid, char *video_path, int loop)
{
    video_queue_delete_player(xuid);
    video_queue_array = (struct video_queue *)
        realloc(video_queue_array, (video_queue_size + 1) * sizeof(struct video_queue));
    struct video_queue *node = &video_queue_array[video_queue_size++];

    if (node == NULL) {
        server_logger("Failed to allocate memory for new node.", ERR);
        return false;
    }
    int total_frames;
    char **filenames = get_filenames(video_path, &total_frames);
    free_filenames(filenames, total_frames);

    node->xuid = xuid;
    node->start_time = clock();
    strncpy(node->video_path, video_path, sizeof(node->video_path));
    node->total_frames = total_frames;
    node->current_frame = 1;
    node->loop = loop;
    node->image = NULL;


    PTP_WORK work = CreateThreadpoolWork(get_video_frame, (PVOID)node, NULL);
    if (work == NULL) {
        CloseThreadpool(thread_pool);
        return false;
    }
    SubmitThreadpoolWork(work);
    node->thread_pool_work = work;

    return true;
}

void reset_screen_pos(void)
{
    start_pos.x = 255;
    start_pos.y = -255;
    start_pos.z = 255;
    end_pos.x = -255;
    end_pos.y = 255;
    end_pos.z = -255;
}

void video_queue_delete_player(long long xuid)
{
    reset_screen_pos();
    for (int i = 0; i < video_queue_size; i++) {
        if (video_queue_array[i].xuid == xuid) {
            CloseThreadpoolWork(video_queue_array[i].thread_pool_work);
            unsigned char *image = video_queue_array[i].image;
            video_queue_array[i].image = NULL;
            free(image);
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

void play_video(struct video_queue *node, struct map_item_saved_data *map_data, struct screen_pos *screen_pos)
{
    struct music_queue_node *music_queue_node = music_queue_get_player(node->xuid);
    if (music_queue_node && music_queue_node->note_queue_node == music_queue_node->note_queue_node_start)
        return;
    char player_xuid[PLAYER_XUID_STR_LEN];
    sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", node->xuid);
    struct player *player = get_player_by_xuid(player_xuid);

    if (((time_t)clock() - node->start_time) <= 0)
        return;
    struct start_pixel start_pixel = {abs(screen_pos->x * 128), abs(screen_pos->y * 128)};

    // check if out of screen
    if (start_pixel.x + 128 > node->ihdr.width || start_pixel.y + 128 > node->ihdr.height)
        return;

    // fill pixels to map
    set_pixels(node->image, map_data, &start_pixel, &node->ihdr);
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
        if (player_xuid == -1)
            return &video_queue_array[i];
        if (video_queue_array[i].xuid == player_xuid)
            return &video_queue_array[i];
    }
    return NULL;
}
