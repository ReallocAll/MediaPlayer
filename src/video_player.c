#include <mediaplayer/video_player.h>

struct block_pos start_pos = {INT_MAX, INT_MIN, INT_MAX};
struct block_pos end_pos = {INT_MIN, INT_MAX, INT_MIN};

struct video_queue *video_queue_array = NULL;
int video_queue_size = 0;

void get_video_frame(void *arg)
{
    struct video_queue *node = (struct video_queue *)arg;
    FILE *fp;
    char filepath[8192];

    sprintf(filepath, "%s/%09d.png", node->video_path, 1);
    fp = fopen(filepath, "rb");
    if (!fp || !get_pixels(fp, &node->ihdr, NULL, true)) {
        if (fp)
            fclose(fp);
        return;
    }
    fclose(fp);

    node->image = malloc(node->ihdr.width * node->ihdr.height * 4);
    if (!node->image)
        return;

    while (is_player_init(node->player)) {
        int frame_index = (int)((((uv_hrtime()) - node->start_time) / UV_HRT_PER_MS) / 50) + 1;

        if (frame_index == node->current_frame) {
            uv_sleep(1);
            continue;
        }

        sprintf(filepath, "%s/%09d.png", node->video_path, frame_index);
        fp = fopen(filepath, "rb");
        if (!fp) {
            if (node->loop > 1) {
                --node->loop;
                node->current_frame = 1;
                node->start_time = uv_hrtime();
                continue;
            }
            video_queue_delete_player(node->player);
            break;
        }

        if (!get_pixels(fp, &node->ihdr, node->image, false)) {
            fclose(fp);
            continue;
        }
        node->current_frame = frame_index;
        fclose(fp);
    }
    free(node->image);
    node->image = NULL;
}

bool video_queue_add_player(struct player *player, char *video_path, int loop)
{
    struct video_queue *new_array;
    int total_frames;
    char **filenames;

    video_queue_delete_player(player);

    new_array = realloc(video_queue_array,
                        (video_queue_size + 1) * sizeof(struct video_queue));
    if (!new_array) {
        server_logger(LOG_LEVEL_ERR, "Failed to allocate memory for new node.");
        return false;
    }
    video_queue_array = new_array;

    struct video_queue *node = &video_queue_array[video_queue_size++];

    filenames = get_filenames(video_path, &total_frames);
    free_filenames(filenames, total_frames);

    node->player = player;
    node->start_time = uv_hrtime();
    strncpy(node->video_path, video_path, sizeof(node->video_path));
    node->total_frames = total_frames;
    node->current_frame = 1;
    node->loop = loop;
    node->image = NULL;
    node->deleted = false;

    uv_thread_t tid;
    uv_thread_create(&tid, get_video_frame, (void *)node);

    return true;
}

void reset_screen_pos(void)
{
    start_pos.x = INT_MAX;
    start_pos.y = INT_MIN;
    start_pos.z = INT_MAX;
    end_pos.x = INT_MIN;
    end_pos.y = INT_MAX;
    end_pos.z = INT_MIN;
}

void video_queue_delete_player(struct player *player)
{
    reset_screen_pos();
    for (int i = 0; i < video_queue_size; i++) {
        if (video_queue_array[i].player == player) {
            struct video_queue *new_array;

            video_queue_array[i].deleted = true;
            uv_sleep(10);
            for (int j = i; j < video_queue_size - 1; j++)
                video_queue_array[j] = video_queue_array[j + 1];

            new_array = realloc(video_queue_array,
                                (video_queue_size - 1) * sizeof(struct video_queue));
            if (new_array || video_queue_size - 1 == 0)
                video_queue_array = new_array;
            video_queue_size--;
            return;
        }
    }
}

void play_video(struct video_queue *node, struct map_item_saved_data *map_data, struct screen_pos *screen_pos)
{
    if (node->deleted)
        return;

    if (((time_t)uv_hrtime() - node->start_time) <= 0)
        return;
    struct start_pixel start_pixel = {abs(screen_pos->x * 128), abs(screen_pos->y * 128)};

    // check if out of screen
    if (start_pixel.x + 128 > node->ihdr.width || start_pixel.y + 128 > node->ihdr.height)
        return;

    // fill pixels to map
    set_pixels(node->image, map_data, &start_pixel, &node->ihdr);
}

struct video_queue *video_queue_get_player(struct player *player)
{
    if (!player)
        return video_queue_array;
    for (int i = 0; i < video_queue_size; i++) {
        if (video_queue_array[i].player == player)
            return &video_queue_array[i];
    }
    return NULL;
}