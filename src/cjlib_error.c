#include <threads.h>

#include "cjlib_error.h"

static struct cjlib_json_error g_error;
static mtx_t g_error_mtx;

int cjlib_json_error_init(void)
{
    (void) memset(&g_error, 0x0, sizeof(struct cjlib_json_error));
    g_error.c_error_code = NO_ERROR;

    if (thrd_error == mtx_init(&g_error_mtx, mtx_plain)) return -1;

    return 0;
}

void cjlib_json_error_destroy(void)
{
    free(g_error.c_property_name);
    free(g_error.c_property_value);

    mtx_destroy(&g_error_mtx);
}

void cjlib_json_get_error(struct cjlib_json_error *restrict dst)
{
    (void) memcpy(dst, &g_error, sizeof(struct cjlib_json_error));
}

void cjlib_setup_error
(const char *property_name, const char *property_value,
 enum cjlib_json_error_types error_code)
{
    // Lock the mutex, in order to be thread safe.
    if (thrd_error == mtx_lock(&g_error_mtx)) return;

    g_error.c_property_name  = strdup(property_name);
    g_error.c_property_value = strdup(property_value);
    g_error.c_error_code     = error_code;
    // Unlock the mutex.
    if (thrd_error == mtx_unlock(&g_error_mtx)) return;
}

enum cjlib_json_error_types cjlib_error_indicator_correction(int func_err_code)
{
    if (NO_ERROR == g_error.c_error_code && func_err_code < 0) {
        g_error.c_error_code = UNDEFINED;
        return UNDEFINED;
    }
    return NO_ERROR;
}
