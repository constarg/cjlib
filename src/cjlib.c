// @file: cjlib.c

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "cjlib.h"
#include "cjlib_dictionary.h"

#define DOUBLE_QUOTES         (0x22) // ASCII representive of "
#define CURLY_BRACKETS_OPEN   (0x7B) // ASCII representive of { 
#define SQUARE_BRACKETS_OPEN  (0x5B) // ASCII representive of [
#define CURLY_BRACKETS_CLOSE  (0x7D) // ASCII representive of } 
#define SQUARE_BRACKETS_CLOSE (0x5D) // ASCII representive of ]


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

// TODO - make a structure which represents an error in a json.
// TODO - after that make a function that return something that represent the error (like the property in which the error occured)

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
    } else return -1; // TODO - Return something else, like the name of the property in which the error had occured.

    if (value_type == CJLIB_NUMBER && (LONG_MAX == value.c_num || LONG_MIN == value.c_num)) return -1;

    property.c_datatype = value_type;
    property.c_value    = value;

    free(property_value); // TODO - make a revision, is it actaully mallocted? when you decide, remove or let it.
    return 0;
}

static inline void decode_simple_property(const struct cjlib_json *restrict src)
{
    struct raw_simple_property raw_property;


}

static inline void decode_complex_property(const struct cjlib_json *restrict src)
{

}

int cjlib_json_read(struct cjlib_json *restrict dst)
{
    unsigned char curr_byte;
    int test = 0;

    do {
        curr_byte = (unsigned char) fgetc(dst->c_fp);
        if (feof(dst->c_fp)) break;

        if (DOUBLE_QUOTES == curr_byte) { // Check  for "
            decode_simple_property((const struct cjlib_json *) dst);
        }

    } while (1);
    
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
