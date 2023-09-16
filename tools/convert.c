#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int width, height, rate;
    char cmd[256];

    printf("Height(4): ");
    scanf("%d", &height);

    printf("Width(7): ");
    scanf("%d", &width);

    printf("Rate(1000): ");
    scanf("%d", &rate);

    int x = width * 128;
    int y = height * 128;

    system("mkdir output");
    remove("output.mp4");
    snprintf(cmd, sizeof(cmd), "ffmpeg.exe -i input.mp4 -r 20 -vf scale=%d:%d -b:v %dk -c:v libx264 -preset medium -c:a aac output.mp4", x, y, rate);
    system(cmd);

    snprintf(cmd, sizeof(cmd), "ffmpeg.exe -i output.mp4 -vf fps=20,scale=%d:%d:flags=lanczos,pad=%d:%d:(ow-iw)/2:(oh-ih)/2:color=black,format=rgba output/%%09d.png", x, y, x, y);
    system(cmd);
    system("pause");

    return 0;
}
