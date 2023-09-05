#include <mediaplayer/video_player.h>

struct video_queue *video_queue_array = NULL;
int video_queue_size = 0;

bool video_queue_add_player(long long xuid, char *video_path, int loop)
{
    video_queue_delete_player(xuid);
    struct video_queue *new_node = (struct video_queue *)malloc(sizeof(struct video_queue));
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

// copy from IDA
// void __fastcall MapItemSavedData::setPixelDirty(MapItemSavedData *this, unsigned int a2, unsigned int a3)
void set_pixel_dirty(struct map_item_saved_data *this, unsigned int row, unsigned int line)
{
    __int64 *v4;      // r11
    __int64 *i;       // r9
    __int64 v6;       // rax
    unsigned int v7;  // edx
    bool v8;          // cf
    unsigned int v9;  // edx
    bool v10;         // cc
    unsigned int v11; // edx
    unsigned int v12; // edx
    *(WORD *)((char *)this + 121) = 257;
    v4 = (__int64 *)*((uintptr_t *)this + 13);
    for (i = (__int64 *)*((uintptr_t *)this + 12); i != v4; i += 2) {
        v6 = *i;
        if (*(BYTE *)(*i + 32)) {
            v7 = row;
            if (*(DWORD *)(v6 + 36) < row)
                v7 = *(DWORD *)(v6 + 36);
            v8 = *(DWORD *)(v6 + 40) < line;
            *(DWORD *)(v6 + 36) = v7;
            v9 = line;
            if (v8)
                v9 = *(DWORD *)(v6 + 40);
            v10 = *(DWORD *)(v6 + 44) <= row;
            *(DWORD *)(v6 + 40) = v9;
            v11 = row;
            if (!v10)
                v11 = *(DWORD *)(v6 + 44);
            v10 = *(DWORD *)(v6 + 48) <= line;
            *(DWORD *)(v6 + 44) = v11;
            v12 = line;
            if (!v10)
                v12 = *(DWORD *)(v6 + 48);
            *(DWORD *)(v6 + 48) = v12;
        } else {
            *(BYTE *)(v6 + 32) = 1;
            *(DWORD *)(v6 + 36) = row;
            *(DWORD *)(v6 + 40) = line;
            *(DWORD *)(v6 + 44) = row;
            *(DWORD *)(v6 + 48) = line;
        }
    }
}

void play_video(struct video_queue *video_queue_node, struct map_item_saved_data *map_data, struct screen_pos *screen_pos)
{
    struct music_queue_node *music_queue_node = music_queue_get_player(video_queue_node->xuid);
    if (music_queue_node && music_queue_node->note_queue_node == music_queue_node->note_queue_node_start)
        return;
    char player_xuid[PLAYER_XUID_STR_LEN];
    sprintf_s(player_xuid, PLAYER_XUID_STR_LEN, "%lld", video_queue_node->xuid);
    struct player *player = get_player_by_xuid(player_xuid);
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

    // get start pixel
    struct spng_ihdr ihdr;
    struct start_pixel start_pixel = {abs(screen_pos->x * 128), abs(screen_pos->y * 128)};
    get_pixels(filepath, &ihdr, true);

    // check if out of screen
    if (start_pixel.x + 128 > ihdr.width || start_pixel.y + 128 > ihdr.height)
        return;

    // fill pixels to map
    unsigned char *image = get_pixels(filepath, &ihdr, false);
    set_pixels(image, map_data, &start_pixel, &ihdr);
    free(image);
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
        if (player_xuid == -1)
            return &video_queue_array[i];
        if (video_queue_array[i].xuid == player_xuid)
            return &video_queue_array[i];
    }
    return NULL;
}
