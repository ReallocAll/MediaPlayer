#pragma once

enum log_level {
    LOG_LEVEL_TRACE = 6u,
    LOG_LEVEL_INFO = 2u,
    LOG_LEVEL_WARN = 4u,
    LOG_LEVEL_ERR = 8u
};

void server_logger(enum log_level level, const char *msg, ...);
