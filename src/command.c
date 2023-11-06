#include <mediaplayer/command.h>


bool proc_mpm_cmd(struct player *player, int argc, const char *argv[], char ***filenames, int *file_count)
{
    if (argc == 1) {
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Usage:\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Before playing music,\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] please use `/mpm list` to get the music list first.\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] /mpm list [name: string]\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] /mpm play <index: number> [loop: number] [music bar type: number]\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] /mpm stop\n");
        return false;
    }
    *filenames = get_filenames(data_path_nbs, file_count);

    char msg[1024];
    if (!*filenames) {
        send_text_packet(player, TEXT_TYPE_RAW, "§c[MediaPlayer] No playable NBS music!\n");
        return false;
    }
    if (strcmp(argv[1], "list") == 0 && argc >= 2 && argc <= 3) {
        send_text_packet(player, TEXT_TYPE_RAW, "§a[MediaPlayer]§6[Index]§b Music List\n");
        for (int index = 0; index < *file_count; index++) {
            if (argc == 3 && !strstr((*filenames)[index], argv[2]))
                continue;
            sprintf(msg, "§a[MediaPlayer]§6[%d]§b %s\n", index, (*filenames)[index]);
            send_text_packet(player, TEXT_TYPE_RAW, msg);
        }
        return false;
    } else if (strcmp(argv[1], "play") == 0 && argc >= 3 && argc <= 5) {
        int file_index = atoi(argv[2]);
        int loop = 1;
        int music_bar_type = MUSIC_BAR_TYPE_BOSS_BAR;
        if (argc == 4)
            loop = atoi(argv[3]);
        if (argc == 5)
            music_bar_type = atoi(argv[4]);
        if (file_index >= 0 && file_index < *file_count) {
            if (music_queue_add_player(player, (*filenames)[file_index], loop, music_bar_type)) {
                char msg[4096];
                sprintf(msg, "§a[MediaPlayer] Now playing music§b %s\n", (*filenames)[file_index]);
                send_text_packet(player, TEXT_TYPE_RAW, msg);
                return false;
            }
        }
    } else if (strcmp(argv[1], "stop") == 0 && argc == 2) {
        send_text_packet(player, TEXT_TYPE_RAW, "§a[MediaPlayer] Stopped.\n");
        music_queue_delete_player(player);
        return false;
    }
    return true;
}


bool proc_mpv_cmd(struct player *player, int argc, const char *argv[], char ***foldernames, int *folder_count)
{
    if (argc == 1) {
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Usage:\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] Before playing video,\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] please use `/mpv list` to get the video list first.\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] /mpv list [name: list]\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] /mpv play <index: number> [loop: number]\n");
        send_text_packet(player, TEXT_TYPE_RAW, "§6[MediaPlayer] /mpv stop\n");
        return false;
    }
    *foldernames = get_foldernames(data_path_video, folder_count);

    char msg[1024];
    if (!*foldernames) {
        send_text_packet(player, TEXT_TYPE_RAW, "§c[MediaPlayer] No playable video!\n");
        return false;
    }
    if (strcmp(argv[1], "list") == 0 && argc >= 2 && argc <= 3) {
        send_text_packet(player, TEXT_TYPE_RAW, "§a[MediaPlayer]§6[Index]§b Video List\n");
        for (int index = 0; index < *folder_count; index++) {
            if (argc == 3 && !strstr((*foldernames)[index], argv[2]))
                continue;
            sprintf(msg, "§a[MediaPlayer]§6[%d]§b %s\n", index, (*foldernames)[index]);
            send_text_packet(player, TEXT_TYPE_RAW, msg);
        }
        return false;
    } else if (strcmp(argv[1], "play") == 0 && argc >= 3 && argc <= 4) {
        int folder_index = atoi(argv[2]);
        int loop = 1;
        if (argc == 4)
            loop = atoi(argv[3]);
        if (folder_index >= 0 && folder_index < *folder_count) {
            char video_path[4096];
            sprintf(video_path, "%s/%s", data_path_video, (*foldernames)[folder_index]);
            sprintf(msg, "§a[MediaPlayer] Now playing video§b %s\n", (*foldernames)[folder_index]);
            send_text_packet(player, TEXT_TYPE_RAW, msg);
            video_queue_add_player(player, video_path, loop);
            return false;
        }
    } else if (strcmp(argv[1], "stop") == 0 && argc == 2) {
        send_text_packet(player, TEXT_TYPE_RAW, "§a[MediaPlayer] Stopped.\n");
        video_queue_delete_player(player);
        return false;
    }
    return true;
}


bool process_cmd(struct player *player, const char *cmd)
{
    char *cmd_m = strdup(cmd);

    const char *argv[5];
    int argc = 0;
    char *token = strtok(cmd_m, " ");
    while (token != NULL && argc < 5) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    bool ret = true;
    if (strcmp(argv[0], "/mpm") == 0) {
        char **filenames = NULL;
        int file_count = 0;
        ret = proc_mpm_cmd(player, argc, argv, &filenames, &file_count);
        if (file_count > 0)
            free_filenames(filenames, file_count);
    } else if (strcmp(argv[0], "/mpv") == 0) {
        char **foldernames = NULL;
        int folder_count = 0;
        ret = proc_mpv_cmd(player, argc, argv, &foldernames, &folder_count);
        if (folder_count > 0)
            free_filenames(foldernames, folder_count);
    }

    free(cmd_m);
    return ret;
}
