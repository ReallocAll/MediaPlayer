#pragma once

#ifdef PLUGIN_VERSION
    #define PLUGIN_VERSION_MSG " Version: " PLUGIN_VERSION
#else
    #define PLUGIN_VERSION_MSG ""
    #define PLUGIN_VERSION ""
#endif

#ifdef __linux__
void init() __attribute__((constructor));
#else
void init();
#endif

void create_plugin_dir(void);
