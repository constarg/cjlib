// @file: cjlib.c

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "cjlib.h"
#include "cjlib_error.h"
#include "cjlib_dictionary.h"

#define DOUBLE_QUOTES         (0x22) // ASCII representive of "
#define CURLY_BRACKETS_OPEN   (0x7B) // ASCII representive of { 
#define SQUARE_BRACKETS_OPEN  (0x5B) // ASCII representive of [
#define CURLY_BRACKETS_CLOSE  (0x7D) // ASCII representive of } 
#define SQUARE_BRACKETS_CLOSE (0x5D) // ASCII representive of ]
#define COMMMA                (0x2C) // ASCII representive of ,

#define DOUBLE_QUOTES_LIMIT   (0x4)  // How many double auotes can be in a simple property value.

enum byte_gathering_opt
{
    DONT_GATHER,
    GETHER 
};

struct raw_simple_property
{
    char *p_name;   // The raw name of the property.
    char *p_value;  // The raw value of the property (as it is in the file)
};

/**
 * This structure represent an object which is not decoded yet.
 */
struct incomplete_property
{
    long int i_value_position; // The position, in file, where the property value is discovered.
    char *i_name;              // The name of the property which holds the object.
};

static struct cjlib_json_error g_error;

void cjlib_json_get_error(struct cjlib_json_error *restrict dst)
{
    (void)memcpy(dst, &g_error, sizeof(struct cjlib_json_error));
}

int cjlib_json_object_set(cjlib_json_object *src, const char *restrict key, 
                          struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype)
{
    value->c_datatype = datatype;
    if (-1 == cjlib_dict_insert(value, &src, key)) return -1;
    return 0;
}

int cjlib_json_object_get(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src,
                          const char *restrict key)
{
    struct cjlib_json_data *tmp = NULL;

    if (-1 == cjlib_dict_search(tmp, src, key)) return -1;

    (void)memcpy(dst, tmp, sizeof(struct cjlib_json_data));
    return 0;
}

int cjlib_json_object_remove(struct cjlib_json_data *restrict dst, cjlib_json_object *src, 
                             const char *restrict key)
{
    // dst == NULL, then you can skip the return value.
    if (NULL == dst) goto perform_deletion;

    if (-1 == cjlib_json_object_get(dst, src, key)) return -1;

perform_deletion:
    if (-1 == cjlib_dict_remove(&src, key)) return -1;

    return 0;
}

int cjlib_json_open(struct cjlib_json *restrict dst, const char *restrict json_path,
                    const char *restrict modes)
{
    FILE *fp = fopen(json_path, modes);
    if (NULL == fp) return -1;

    dst->c_fp = fp;
    cjlib_dict_init(dst->c_dict);
    cjlib_json_error_init(&g_error);
    return 0;
}

void cjlib_json_close(struct cjlib_json *restrict src)
{
    cjlib_dict_destroy(src->c_dict);
    fclose(src->c_fp);
}

static CJLIB_ALWAYS_INLINE bool is_number(const char *restrict src)
{
    while (*src) {
        if (isdigit(*src++) == 0) return false;
    }
    return true;
}

static CJLIB_ALWAYS_INLINE void setup_error(const char *property_name, const char *property_value, 
                                            enum cjlib_json_error_types error_code)
{
    g_error.c_property_name  = strdup(property_name);
    g_error.c_property_value = strdup(property_value);
    g_error.c_error_code     = error_code;
}

static inline int identify_simple_property(const struct raw_simple_property *restrict src)
{
    char *property_value = src->p_value;
    size_t property_len = strlen(property_value);

    enum cjlib_json_datatypes value_type;
    union cjlib_json_data_disting value;
    struct cjlib_json_data property;

    // build the data that is going to be inserted in the json.
    if (DOUBLE_QUOTES == property_value[0] && DOUBLE_QUOTES == property_value[property_len - 1]) { // Check for "
        value_type = CJLIB_STRING;
        value.c_str = strdup(property_value);
    } else if (!strcmp(property_value, "true") || !strcmp(property_value, "false")) {
        value_type = CJLIB_BOOLEAN;
        value.c_boolean = (!strcmp(property_value, "true"))? true:false;
    } else if (is_number(property_value)) {
        value_type = CJLIB_NUMBER;
        value.c_num = strtol(property_value, NULL, 10);
    } else {
        setup_error(src->p_name, src->p_value, INVALID_TYPE);
        return -1;
    }

    // TODO - check whether to free the memory of property name and value.

    if (value_type == CJLIB_NUMBER && (LONG_MAX == value.c_num || LONG_MIN == value.c_num)) {
        setup_error(src->p_name, src->p_value, INVALID_NUMBER);
        return -1;
    }
    property.c_datatype = value_type;
    property.c_value    = value;

    
    return 0;
}

static inline void decode_simple_property_value(const struct cjlib_json *restrict src, const char *p_name)
{
    struct raw_simple_property raw_property;

    // unsigned char property_byte;
    // do {
    //     property_byte = (unsigned char) fgetc(src->c_fp);
    //     if (feof(src->c_fp)) return -1; // It is an error. 

    //     if (CURLY_BRACKETS_CLOSE == property_byte || COMMMA == property_byte) {
    //         // TODO - here the property is complete.
    //     }


    // } while 

}

static inline void decode_complex_property_value(const struct cjlib_json *restrict src, const char *p_name)
{

}

static inline void decode_property(const struct cjlib_json *restrict src)
{

}

static unsigned char *search_byte_until(FILE *src, unsigned char ch_stop, enum byte_gathering_opt gather)
{
    unsigned char curr_byte;
    unsigned char *chunk = NULL; 
    size_t chunk_init_s = 128;
    size_t chunk_s = 0;

    if (gather == GETHER) {
        chunk = (unsigned char *) malloc(sizeof(char) * chunk_init_s);
        if (NULL == chunk) return NULL;
    }

    do {
        curr_byte = (unsigned char) fgetc(src);
        if (feof(src)) return 0;

        if (gather == GETHER) {
            chunk[chunk_s++] = curr_byte;
            if (chunk_s == chunk_init_s) {
                chunk_init_s += chunk_init_s;
                chunk = (unsigned char *) realloc(chunk, sizeof(char) * chunk_init_s);
                if (NULL == chunk) return NULL;
            }
        }

        if (ch_stop == curr_byte) break;

    } while (1);

    return chunk;
}

static inline int validate_json(const struct cjlib_json *restrict src)
{
    unsigned char curr_byte;
    int curl_brackets_count   = 0;
    int square_brackets_count = 0;
    int double_quotes_count   = 0;

    do {
        curr_byte = (unsigned char) fgetc(src->c_fp);
        if (feof(src->c_fp)) break;

        if (CURLY_BRACKETS_OPEN == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte) ++curl_brackets_count;
        if (SQUARE_BRACKETS_OPEN == curr_byte || SQUARE_BRACKETS_CLOSE == curr_byte) ++square_brackets_count;
        if (DOUBLE_QUOTES == curr_byte) ++double_quotes_count;

    } while (1);

    rewind(src->c_fp);

    if (curl_brackets_count % 2 != 0) setup_error("", "", INCOMPLETE_CRULY_BRACKETS);
    else if (square_brackets_count % 2 != 0) setup_error("", "", INCOMPLETE_SQAURE_BRACKETS);
    else if (double_quotes_count % 2 != 0) setup_error("", "", INCOMPLETE_DOUBLE_QUOTES);
    
    if (g_error.c_error_code != NO_ERROR) return -1;

    return 0;
}

int cjlib_json_read(struct cjlib_json *restrict dst)
{
    unsigned char curr_byte;

    if (validate_json(dst) == 0) {
        printf("CORRECT\n");
    } else {
        printf("WRONG!!!\n");
    }

    // while (1) {
    //     (void)search_byte_until(dst->c_fp, DOUBLE_QUOTES, DONT_GATHER);
    //     if (feof(dst->c_fp)) break;

    //     decode_property((const struct cjlib_json *) dst);
    // }

    return 0;
}

char *cjlib_json_object_stringtify(const cjlib_json_object *restrict src)
{
    return NULL;
}

int cjlib_json_dump(const struct cjlib_json *restrict src)
{
    // 1. Translate the dicionary from the memory to json format.
    // 2. Write the resulted string in the json file.
    return 0;
}
