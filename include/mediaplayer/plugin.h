#pragma once
#pragma comment(lib, "littlehooker.lib")
#include <littlehooker/littlehooker.h>
#include <time.h>

#include "LLPreloaderAPI.h"
#include "cpp_string.h"
#include "logger.h"
#include "music_player.h"
#include "command.h"

#include "mc/network.h"
#include "mc/actor.h"
#include "mc/player.h"
#include "mc/level.h"

#ifdef PLUGIN_VERSION
    #define PLUGIN_VERSION_MSG " Version: "
#else
    #define PLUGIN_VERSION_MSG ""
    #define PLUGIN_VERSION ""
#endif

bool check_ll_preloader(void);
bool init_hooks(void);
void create_plugin_dir(void);
bool load_plugin(void);
bool unload_plugin(void);
