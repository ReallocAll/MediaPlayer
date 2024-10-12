#pragma once

#define PLUGIN_NAME "MediaPlayer"

#define LEVEL_ERROR 0
#define LEVEL_WARN 1
#define LEVEL_INFO 2
#define LEVEL_DEBUG 3
#define LEVEL_TRACE 4

#ifndef LOG_LEVEL
#define LOG_LEVEL LEVEL_INFO
#endif

#if LEVEL_ERROR <= LOG_LEVEL
#define LB_PREINIT_LOGGER_ERROR(in_fmt, ...) \
	lb_preinit_logger(LEVEL_ERROR, PLUGIN_NAME, in_fmt, __VA_ARGS__)
#else
#define LB_PREINIT_LOGGER_ERROR(in_fmt, ...)
#endif

#if LEVEL_WARN <= LOG_LEVEL
#define LB_PREINIT_LOGGER_WARN(in_fmt, ...) \
	lb_preinit_logger(LEVEL_WARN, PLUGIN_NAME, in_fmt, __VA_ARGS__)
#else
#define LB_PREINIT_LOGGER_WARN(in_fmt, ...)
#endif

#if LEVEL_INFO <= LOG_LEVEL
#define LB_PREINIT_LOGGER_INFO(in_fmt, ...) \
	lb_preinit_logger(LEVEL_INFO, PLUGIN_NAME, in_fmt, __VA_ARGS__)
#else
#define LB_PREINIT_LOGGER_INFO(in_fmt, ...)
#endif

#if LEVEL_DEBUG <= LOG_LEVEL
#define LB_PREINIT_LOGGER_DEBUG(in_fmt, ...) \
	lb_preinit_logger(LEVEL_DEBUG, PLUGIN_NAME, in_fmt, __VA_ARGS__)
#else
#define LB_PREINIT_LOGGER_DEBUG(in_fmt, ...)
#endif

#if LEVEL_TRACE <= LOG_LEVEL
#define LB_PREINIT_LOGGER_TRACE(in_fmt, ...) \
	lb_preinit_logger(LEVEL_TRACE, PLUGIN_NAME, in_fmt, __VA_ARGS__)
#else
#define LB_PREINIT_LOGGER_TRACE(in_fmt, ...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

char lb_sym_avail();
void *lb_sym_find(const char *in_sym);

void *lb_ahook_install(void *in_target, void *in_hook, void **out_call, char in_priority);
void *lb_shook_install(const char *in_sym, void *in_hook, void **out_call, char in_priority);
void *lb_ohook_install(const char *in_fmt, int in_offset, void *in_hook, void **out_call, char in_priority);
void lb_hook_uninstall(const char *in_sym, void *in_target, void *in_call);

int lb_preinit_logger(int in_level, const char *in_perfix, const char *in_fmt, ...);
unsigned char *lb_get_base_addr();

void *lb_opt_search(const char *in_fmt, int in_offset);

#ifdef __cplusplus
}
#endif

// ----- HOOK -----

struct lb_hook {
	void *addr;
	void *(*install)();
	void *(*installex)(unsigned char in_priority);
	void (*uninstall)();
	void *call;
	void *hook;
};

#define AHOOK(name, ret_type, i_addr, ...)                                                        \
	void *lb_hook_##name##_install();                                                             \
	void *lb_hook_##name##_installex(char in_priority);                                           \
	void lb_hook_##name##_uninstall();                                                            \
	ret_type lb_hook_##name##_hook(__VA_ARGS__);                                                  \
                                                                                                  \
	struct lb_hook_##name {                                                                       \
		void *addr;                                                                               \
		void *(*install)();                                                                       \
		void *(*installex)(char);                                                                 \
		void (*uninstall)();                                                                      \
		ret_type (*call)(__VA_ARGS__);                                                            \
		ret_type (*hook)(__VA_ARGS__);                                                            \
	} name = { NULL,                                                                              \
			   lb_hook_##name##_install,                                                          \
			   lb_hook_##name##_installex,                                                        \
			   lb_hook_##name##_uninstall,                                                        \
			   (ret_type(*)(__VA_ARGS__))NULL,                                                    \
			   lb_hook_##name##_hook };                                                           \
                                                                                                  \
	void *lb_hook_##name##_install()                                                              \
	{                                                                                             \
		return name.addr = lb_ahook_install(i_addr, name.hook, (void **)&name.call, 127);         \
	}                                                                                             \
	void *lb_hook_##name##_installex(char in_priority)                                            \
	{                                                                                             \
		return name.addr = lb_ahook_install(i_addr, name.hook, (void **)&name.call, in_priority); \
	}                                                                                             \
	void lb_hook_##name##_uninstall()                                                             \
	{                                                                                             \
		lb_hook_uninstall(#i_addr, name.addr, (void *)name.hook);                                 \
	}                                                                                             \
	ret_type lb_hook_##name##_hook(__VA_ARGS__)

#define SHOOK(name, ret_type, sym, ...)                                                        \
	void *lb_hook_##name##_install();                                                          \
	void *lb_hook_##name##_installex(char in_priority);                                        \
	void lb_hook_##name##_uninstall();                                                         \
	ret_type lb_hook_##name##_hook(__VA_ARGS__);                                               \
                                                                                               \
	struct lb_hook_##name {                                                                    \
		void *addr;                                                                            \
		void *(*install)();                                                                    \
		void *(*installex)(char);                                                              \
		void (*uninstall)();                                                                   \
		ret_type (*call)(__VA_ARGS__);                                                         \
		ret_type (*hook)(__VA_ARGS__);                                                         \
	} name = { NULL,                                                                           \
			   lb_hook_##name##_install,                                                       \
			   lb_hook_##name##_installex,                                                     \
			   lb_hook_##name##_uninstall,                                                     \
			   (ret_type(*)(__VA_ARGS__))NULL,                                                 \
			   lb_hook_##name##_hook };                                                        \
                                                                                               \
	void *lb_hook_##name##_install()                                                           \
	{                                                                                          \
		return name.addr = lb_shook_install(sym, name.hook, (void **)&name.call, 127);         \
	}                                                                                          \
	void *lb_hook_##name##_installex(char in_priority)                                         \
	{                                                                                          \
		return name.addr = lb_shook_install(sym, name.hook, (void **)&name.call, in_priority); \
	}                                                                                          \
	void lb_hook_##name##_uninstall()                                                          \
	{                                                                                          \
		lb_hook_uninstall(sym, name.addr, (void *)name.hook);                                  \
	}                                                                                          \
	ret_type lb_hook_##name##_hook(__VA_ARGS__)

#define OHOOK(name, ret_type, fmt, offset, ...)                                                        \
	void *lb_hook_##name##_install();                                                                  \
	void *lb_hook_##name##_installex(char in_priority);                                                \
	void lb_hook_##name##_uninstall();                                                                 \
	ret_type lb_hook_##name##_hook(__VA_ARGS__);                                                       \
                                                                                                       \
	struct lb_hook_##name {                                                                            \
		void *addr;                                                                                    \
		void *(*install)();                                                                            \
		void *(*installex)(char);                                                                      \
		void (*uninstall)();                                                                           \
		ret_type (*call)(__VA_ARGS__);                                                                 \
		ret_type (*hook)(__VA_ARGS__);                                                                 \
	} name = { NULL,                                                                                   \
			   lb_hook_##name##_install,                                                               \
			   lb_hook_##name##_installex,                                                             \
			   lb_hook_##name##_uninstall,                                                             \
			   (ret_type(*)(__VA_ARGS__))NULL,                                                         \
			   lb_hook_##name##_hook };                                                                \
                                                                                                       \
	void *lb_hook_##name##_install()                                                                   \
	{                                                                                                  \
		return name.addr = lb_ohook_install(fmt, offset, name.hook, (void **)&name.call, 127);         \
	}                                                                                                  \
	void *lb_hook_##name##_installex(char in_priority)                                                 \
	{                                                                                                  \
		return name.addr = lb_ohook_install(fmt, offset, name.hook, (void **)&name.call, in_priority); \
	}                                                                                                  \
	void lb_hook_##name##_uninstall()                                                                  \
	{                                                                                                  \
		lb_hook_uninstall(fmt, name.addr, (void *)name.hook);                                          \
	}                                                                                                  \
	ret_type lb_hook_##name##_hook(__VA_ARGS__)

#define ODHOOK(name, ret_type, fmt, ...) OHOOK(name, ret_type, fmt, 0, __VA_ARGS__)

// ----- CALL -----

#define SCALL(sym, func_proto, ...) \
	((func_proto)lb_sym_find(sym))(__VA_ARGS__)

#define ACALL(addr, func_proto, ...) \
	((func_proto)addr)(__VA_ARGS__)

// ----- MISC -----

#define OFIND(name, opt_code, offset) \
	name = lb_opt_search(opt_code, offset);

#define ODFIND(name, opt_code) \
	OFIND(name, opt_code, 0)

#define VCALL(lb_hook, ...) (lb_hook.call_status->is_called ? (void)0 : lb_hook.call(__VA_ARGS__), lb_hook.call_status->is_called = 1)
#define DCALL(lb_hook, ...) (lb_hook.call_status->is_called ? lb_hook.call_status->ret_value : (lb_hook.call_status->is_called = 1, lb_hook.call_status->ret_value = (void *)(unsigned long long)lb_hook.call(__VA_ARGS__)))
