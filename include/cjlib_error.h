#ifndef CJLIB_ERROR
#define CJLIB_ERROR

#include <memory.h>
#include <malloc.h>

enum cjlib_json_error_types
{
    INVALID_TYPE,
    INVALID_NAME,
    DUPLICATE_NAME,
    INVALID_PROPERTY,
    INCOMPLETE_BRACKETS,
    INCOMPLETE_SQAURE_BRACKETS,
    MISSING_COMMA,
    INVALID_NUMBER
};

struct cjlib_json_error
{
    char *c_property_name;  // The name of the property in which the error occured.
    char *c_property_value; // The value of the property in which the error occured.
    enum cjlib_json_error_types c_error_code; // A error code indicating the error.
};


static inline void cjlib_json_error_init(struct cjlib_json_error *restrict src)
{
    (void)memset(src, 0x0, sizeof(struct cjlib_json_error));
}

static inline void cjlib_json_error_destroy(struct cjlib_json_error *restrict src)
{
    free(src->c_property_name);
    free(src->c_property_value);
}

extern void cjlib_json_get_error(struct cjlib_json_error *restrict dst);

#endif