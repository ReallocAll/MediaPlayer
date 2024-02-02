#pragma once
#include <time.h>

#include <libcutils/libcutils.h>
#include <mediaplayer/mc/symbols.h>

#include "logger.h"
#include "music_player.h"
#include "command.h"
#include "event_handler.h"
#include "file_utils.h"

#include "mc/structs.h"

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

#ifdef __linux__
void init() __attribute__((constructor));
#else
void init();
#endif

void create_plugin_dir(void);
