#include <mediaplayer/plugin.h>

bool load_plugin(void);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        if (!lh_init()) {
            puts("LittleHooker init failed\n");
            break;
        }
        load_plugin();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        lh_uninit();
        break;
    }
    return true;
}

bool load_plugin(void)
{
    init_hooks();

    return true;
}
