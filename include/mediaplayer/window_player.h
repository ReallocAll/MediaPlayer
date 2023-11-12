#include <windows.h>
#include <mediaplayer/process_png.h>

HWND create_playback_window(struct spng_ihdr *ihdr);
void destroy_playback_window(HWND hwnd);
void update_playback_window(HWND hwnd, struct spng_ihdr *ihdr,
                         unsigned char *image);
