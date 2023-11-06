#include <mediaplayer/logger.h>

void server_logger(enum log_level level, const char *msg, ...)
{
	va_list va;
	va_start(va, msg);
	SYMCALL(S_BedrockLog__log_va,
		char (*)(unsigned int category, char set, int rule, int area, unsigned int level, const char *tag, int tid, const char *format, va_list args),
		0, 1, 0x0d00, 0, level, "私のオナニーを見てください！", 0x0721, msg, va);
}
