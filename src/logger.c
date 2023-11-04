#include <mediaplayer/logger.h>

void server_logger(enum log_level level, const char *msg, ...)
{
	va_list va;
	va_start(va, msg);
	SYMCALL("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
		char (*)(unsigned int a1, char a2, int a3, int a4, unsigned int a5, const char *a6, int a7, const char *a8, va_list a9),
		0, 1, 0x0d00, 0, level, "私のオナニーを見てください！", 0x0721, msg, va);
}
