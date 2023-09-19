#pragma once
#include <littlehooker/littlehooker.h>

enum log_level {
    INFO = 2u,
    WARN = 4u,
    UNKNOWN = 6u,
    ERR = 8u
};

void server_logger(enum log_level level, const char *msg, ...);
