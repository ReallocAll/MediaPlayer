#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_MAP_DIM  20
#define MAX_BITRATE  50000
#define MIN_BITRATE  100
#define CMD_BUF_SIZE 512

static int read_int(const char *prompt, int min, int max)
{
    int val;
    int ret;
    do {
        printf("%s", prompt);
        ret = scanf("%d", &val);
        if (ret != 1) {
            fprintf(stderr, "Invalid input, please enter a number.\n");
            while (getchar() != '\n'); /* flush stdin */
            continue;
        }
        if (val < min || val > max) {
            fprintf(stderr, "Value must be between %d and %d.\n", min, max);
            continue;
        }
        break;
    } while (1);
    return val;
}

int main(void)
{
    int width, height, rate;
    char cmd[CMD_BUF_SIZE];
    int ret;

    height = read_int("Height(4): ", 1, MAX_MAP_DIM);
    width  = read_int("Width(7): ", 1, MAX_MAP_DIM);
    rate   = read_int("Rate(1000): ", MIN_BITRATE, MAX_BITRATE);

    int x = width * 128;
    int y = height * 128;

    ret = system("mkdir output 2>nul");
    (void)ret;
    remove("output.mp4");
    snprintf(cmd, sizeof(cmd),
             "ffmpeg.exe -i input.mp4 -r 20 -vf scale=%d:%d -b:v %dk -c:v libx264 -preset medium -c:a aac output.mp4",
             x, y, rate);
    printf("Running: %s\n", cmd);
    ret = system(cmd);
    if (ret != 0)
        fprintf(stderr, "ffmpeg first pass failed with code %d\n", ret);

    snprintf(cmd, sizeof(cmd),
             "ffmpeg.exe -i output.mp4 -vf fps=20,scale=%d:%d:flags=lanczos,pad=%d:%d:(ow-iw)/2:(oh-ih)/2:color=black,format=rgba output/%%09d.png",
             x, y, x, y);
    printf("Running: %s\n", cmd);
    ret = system(cmd);
    if (ret != 0)
        fprintf(stderr, "ffmpeg second pass failed with code %d\n", ret);

    system("pause");
    return 0;
}
