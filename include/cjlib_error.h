#ifndef CJLIB_ERROR
#define CJLIB_ERROR

#include <memory.h>
#include <malloc.h>

enum cjlib_json_error_types
{
    NO_ERROR,
    UNDEFINED,
    INVALID_TYPE,
    INVALID_NAME,
    INVALID_JSON,
    DUPLICATE_NAME,
    INVALID_PROPERTY,
    MISSING_SEPERATOR,
    MEMORY_ERROR,
    INCOMPLETE_CURLY_BRACKETS,
    INCOMPLETE_SQUARE_BRACKETS,
    INCOMPLETE_DOUBLE_QUOTES,
    MISSING_COMMA,
    INVALID_NUMBER
};

struct cjlib_json_error
{
    char *c_property_name;                    // The name of the property in which the error occurred.
    char *c_property_value;                   // The value of the property in which the error occurred.
    enum cjlib_json_error_types c_error_code; // A error code indicating the error.
};

extern int cjlib_json_error_init(void);

extern void cjlib_json_error_destroy(void);

extern void cjlib_setup_error
(const char *property_name, const char *property_value,
 enum cjlib_json_error_types error_code);

extern void cjlib_json_get_error(struct cjlib_json_error *restrict dst);

extern enum cjlib_json_error_types cjlib_error_indicator_correction(int func_err_code);

#endif
