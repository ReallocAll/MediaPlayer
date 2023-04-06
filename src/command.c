#include <mediaplayer/command.h>

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

    if (strcmp(argv[0], "/mp") == 0 || strcmp(argv[0], "/mediaplayer") == 0) {
        if (argc == 1) {
            send_text_packet(player, 0, "§6[MediaPlayer] Usage:\n");
            send_text_packet(player, 0, "§6[MediaPlayer] Before playing music,\n");
            send_text_packet(player, 0, "§6[MediaPlayer] please use `/mp list` to get the music list first.\n");
            send_text_packet(player, 0, "§6[MediaPlayer] /mp list\n");
            send_text_packet(player, 0, "§6[MediaPlayer] /mp play <index: number>\n");
            send_text_packet(player, 0, "§6[MediaPlayer] /mp stop\n");
            return false;
        }
        char path[260];
        GetCurrentDirectoryA(260, path);
        strcat(path, "\\plugins\\MediaPlayer\\nbs");

        int file_count = 0;
        char** file_names = get_filenames(path, &file_count);
        
        long long player_xuid = atoll(get_player_xuid(player));
        char msg[1024];
        if (!file_names) {
            send_text_packet(player, 0, "§c[MediaPlayer] No playable NBS music!\n");
            return false;
        }
        if (strcmp(argv[1], "list") == 0 && argc == 2) {
            send_text_packet(player, 0, "§a[MediaPlayer]§6[Index]§b Music List\n");
            for (int index = 0; index < file_count; index++) {
                sprintf(msg, "§a[MediaPlayer]§6[%d]§b %s\n", index, file_names[index]);
                send_text_packet(player, 0, msg);
            }
            free_filenames(file_names, file_count);
            return false;
        } else if (strcmp(argv[1], "play") == 0 && argc == 3) {
            int file_index = atoi(argv[2]);
            if (file_index >= 0 && file_index < file_count) {
                strcat(path, "\\");
                strcat(path, file_names[file_index]);
                FILE *fp = fopen(path, "rb");
                if (fp) {
                    sprintf(msg, "§a[MediaPlayer] Now playing§b %s\n", file_names[file_index]);
                    send_text_packet(player, 0, msg);
                    music_queue_add_player(player_xuid, fp);
                    fclose(fp);
                    free_filenames(file_names, file_count);
                    return false;
                }
                free_filenames(file_names, file_count);
            }
        } else if (strcmp(argv[1], "stop") == 0 && argc == 2) {
            send_text_packet(player, 0, "§a[MediaPlayer] Stopped.\n");
            music_queue_delete_player(player_xuid);
            free_filenames(file_names, file_count);
            return false;
        }
        free_filenames(file_names, file_count);
    }
    return true;
}
