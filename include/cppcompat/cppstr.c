#include "cppstr.h"
#include <stdlib.h>
#include <string.h>

#define SSO_CAP  15

static void construct(char *obj, const char *str)
{
    size_t len = strlen(str);
    if (len > CPPSTR_MAX) len = CPPSTR_MAX;

    memset(obj, 0, CPPSTR_SIZE);
    *(size_t *)(obj + 24) = SSO_CAP;

    if (len <= SSO_CAP) {
        memcpy(obj, str, len + 1);
        *(size_t *)(obj + 16) = len;
    } else {
        size_t cap = len | 0xF;
        if (cap < 22) cap = 22;

        char *buf = (char *)malloc(cap + 1);
        if (!buf) {
            memcpy(obj, str, SSO_CAP + 1);
            *(size_t *)(obj + 16) = SSO_CAP;
            return;
        }
        memcpy(buf, str, len + 1);
        *(char **)(obj) = buf;
        *(size_t *)(obj + 16) = len;
        *(size_t *)(obj + 24) = cap;
    }
}

void *cppstr_new(const char *str)
{
    char *obj = (char *)malloc(CPPSTR_SIZE);
    if (!obj) return NULL;
    construct(obj, str ? str : "");
    return obj;
}

void cppstr_place(void *dst, const char *str)
{
    if (!dst) return;
    construct((char *)dst, str ? str : "");
}

const char *cppstr_str(const void *s)
{
    if (!s) return "";
    const char *obj = (const char *)s;
    return *(const size_t *)(obj + 24) <= SSO_CAP
        ? obj
        : *(const char *const *)obj;
}

void cppstr_free(void *s, bool free_obj)
{
    if (!s) return;
    char *obj = (char *)s;
    if (*(size_t *)(obj + 24) > SSO_CAP) {
        char *buf = *(char **)obj;
        if (buf) free(buf);
    }
    if (free_obj) free(obj);
}
