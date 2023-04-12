#include <mediaplayer/command.h>

bool proc_mpm_cmd(struct player *player, int argc, const char *argv[], char ***filenames, int *file_count)
{
    if (argc == 1) {
        send_text_packet(player, 0, "§6[MediaPlayer] Usage:\n");
        send_text_packet(player, 0, "§6[MediaPlayer] Before playing music,\n");
        send_text_packet(player, 0, "§6[MediaPlayer] please use `/mpm list` to get the music list first.\n");
        send_text_packet(player, 0, "§6[MediaPlayer] /mpm list\n");
        send_text_packet(player, 0, "§6[MediaPlayer] /mpm play <index: number>\n");
        send_text_packet(player, 0, "§6[MediaPlayer] /mpm stop\n");
        return false;
    }
    *filenames = get_filenames(data_path_nbs, file_count);

    long long player_xuid = atoll(get_player_xuid(player));
    char msg[1024];
    if (!*filenames) {
        send_text_packet(player, 0, "§c[MediaPlayer] No playable NBS music!\n");
        return false;
    }
    if (strcmp(argv[1], "list") == 0 && argc == 2) {
        send_text_packet(player, 0, "§a[MediaPlayer]§6[Index]§b Music List\n");
        for (int index = 0; index < *file_count; index++) {
            sprintf(msg, "§a[MediaPlayer]§6[%d]§b %s\n", index, (*filenames)[index]);
            send_text_packet(player, 0, msg);
        }
        return false;
    } else if (strcmp(argv[1], "play") == 0 && argc == 3) {
        int file_index = atoi(argv[2]);
        if (file_index >= 0 && file_index < *file_count) {
            if (music_queue_add_player(player_xuid, (*filenames)[file_index])) {
                char msg[260];
                sprintf(msg, "§a[MediaPlayer] Now playing music§b %s\n", (*filenames)[file_index]);
                send_text_packet(player, 0, msg);
                return false;
            }
        }
    } else if (strcmp(argv[1], "stop") == 0 && argc == 2) {
        send_text_packet(player, 0, "§a[MediaPlayer] Stopped.\n");
        music_queue_delete_player(player_xuid);
        return false;
    }
    return true;
}

bool proc_mpv_cmd(struct player *player, int argc, const char *argv[], char ***foldernames, int *folder_count)
{
    if (argc == 1) {
        send_text_packet(player, 0, "§6[MediaPlayer] Usage:\n");
        send_text_packet(player, 0, "§6[MediaPlayer] Before playing video,\n");
        send_text_packet(player, 0, "§6[MediaPlayer] please use `/mpv list` to get the video list first.\n");
        send_text_packet(player, 0, "§6[MediaPlayer] /mpv list\n");
        send_text_packet(player, 0, "§6[MediaPlayer] /mpv play <index: number>\n");
        send_text_packet(player, 0, "§6[MediaPlayer] /mpv stop\n");
        return false;
    }
    *foldernames = get_foldernames(data_path_video, folder_count);

    long long player_xuid = atoll(get_player_xuid(player));
    char msg[1024];
    if (!*foldernames) {
        send_text_packet(player, 0, "§c[MediaPlayer] No playable video!\n");
        return false;
    }
    if (strcmp(argv[1], "list") == 0 && argc == 2) {
        send_text_packet(player, 0, "§a[MediaPlayer]§6[Index]§b Video List\n");
        for (int index = 0; index < *folder_count; index++) {
            sprintf(msg, "§a[MediaPlayer]§6[%d]§b %s\n", index, (*foldernames)[index]);
            send_text_packet(player, 0, msg);
        }
        return false;
    } else if (strcmp(argv[1], "play") == 0 && argc == 3) {
        int folder_index = atoi(argv[2]);
        if (folder_index >= 0 && folder_index < *folder_count) {
            char video_path[260];
            sprintf(video_path, "%s\\%s", data_path_video, (*foldernames)[folder_index]);
            sprintf(msg, "§a[MediaPlayer] Now playing video§b %s\n", (*foldernames)[folder_index]);
            send_text_packet(player, 0, msg);
            video_queue_add_player(player_xuid, video_path);
            return false;
        }
    } else if (strcmp(argv[1], "stop") == 0 && argc == 2) {
        send_text_packet(player, 0, "§a[MediaPlayer] Stopped.\n");
        video_queue_delete_player(player_xuid);
        return false;
    }
    return true;
}

bool process_cmd(struct player *player, const char *cmd)
{
    char *cmd_m = _strdup(cmd);

    const char *argv[3];
    int argc = 0;
    char *token = strtok(cmd_m, " ");
    while (token != NULL && argc < 3) {
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

    return ret;
}
