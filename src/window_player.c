#include <mediaplayer/window_player.h>

#ifdef DEBUG
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND create_playback_window(struct spng_ihdr *ihdr) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = "ARGBWindowClass";

    RegisterClass(&wc);
    
    HWND hwnd = CreateWindowA("ARGBWindowClass", "MediaPlayer", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, ihdr->width, ihdr->height, NULL, NULL, wc.hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    return hwnd;
}

void destroy_playback_window(HWND hwnd)
{
    DestroyWindow(hwnd);
    UnregisterClass("ARGBWindowClass", GetModuleHandle(NULL));
}

void update_playback_window(HWND hwnd, struct spng_ihdr *ihdr, unsigned char *image)
{
    HDC hdc = GetDC(hwnd);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = ihdr->width;
    bmi.bmiHeader.biHeight = -(LONG)(ihdr->height);  // Negative height to indicate top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;  // Assuming 32 bits per pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(
        hdc,                           // destination device context
        0,                             // x-coordinate of the upper-left corner
        0,                             // y-coordinate of the upper-left corner
        ihdr->width,                   // width of the DIB
        ihdr->height,                  // height of the DIB
        0,                             // x-coordinate of the lower-left corner
        0,                             // y-coordinate of the lower-left corner
        0,                             // starting scan line
        ihdr->height,                  // number of scan lines
        image,                         // array of DIB bits
        &bmi,                          // bitmap information
        DIB_RGB_COLORS                 // color table usage
    );

    ReleaseDC(hwnd, hdc);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

#else

HWND create_playback_window(struct spng_ihdr *ihdr) { return NULL; }
void destroy_playback_window(HWND hwnd) {}
void update_playback_window(HWND hwnd, struct spng_ihdr *ihdr, unsigned char *image) {}

#endif
