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
#define WHITE_SPACE           (0x20) // ASCII representive of ' '
#define SEPERATOR             (0x3A) // ASCII representive of :

#define MEMORY_INIT_CHUNK     (0x3C) // Hex representive of 60.
#define EXP_DOUBLE_QUOTES     (0x02) // Expected double quotes.

#define P_VALUE_IS_OBJECT(VALUE_PTR) (*VALUE_PTR == CURLY_BRACKETS_OPEN)
#define P_VALUE_IS_ARRAY(VALUE_PTR) (*VALUE_PTR == SQUARE_BRACKETS_OPEN) 


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
    char *i_pname; // THe name of the incomplete property.
    cjlib_json_object i_object; // The object where it belong
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
    cjlib_json_destroy(src);
    fclose(src->c_fp);
    (void)memset(src, 0x0, sizeof(struct cjlib_json));
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
    } else if (!strcmp(property_value, "null")) {
        value_type = CJLIB_NULL,
        value.c_null = NULL;
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


}

static char *parse_property_name(const struct cjlib_json *restrict src)
{
    unsigned char curr_byte;
    int double_quotes_c = 0;
    bool found_seperator = false;

    size_t p_name_init_s = MEMORY_INIT_CHUNK;
    size_t p_name_s      = 0;
    
    char *p_name = (char *) malloc(sizeof(char) * p_name_init_s);
    if (NULL == p_name) {
        setup_error("", "", MEMORY_ERROR);
        return NULL;
    }

    do {
        curr_byte = (unsigned char) fgetc(src->c_fp);
        if (feof(src->c_fp)) {
            p_name[p_name_s] = '\0';
            setup_error(p_name, "", INVALID_PROPERTY);
            free(p_name);
            return NULL;
        }

        if (WHITE_SPACE == curr_byte) continue; // Check for ' '

        // Check for "
        if (DOUBLE_QUOTES == curr_byte) ++double_quotes_c;
        // Check fof :
        if (SEPERATOR == curr_byte) found_seperator = true;

        if (double_quotes_c > 0) {
            p_name[p_name_s++] = curr_byte;
            if ((p_name_s + 1) == p_name_init_s) {
                p_name_init_s += MEMORY_INIT_CHUNK;
                p_name = (char *) realloc(p_name, sizeof(char) * p_name_init_s);
                if (NULL == p_name) {
                    setup_error("", "", MEMORY_ERROR);
                    return NULL;
                }
            }
        }
        // How many double quotes are expected.
        if (EXP_DOUBLE_QUOTES == double_quotes_c && found_seperator) break;
        if (found_seperator) {
            p_name[p_name_s] = '\0';
            setup_error(p_name, "", MISSING_SEPERATOR);
            free(p_name);
            return NULL;
        }

    } while (1);
    p_name[p_name_s - 1] = '\0'; // -1, to not include the seperator.

    p_name = (char *) realloc(p_name, sizeof(char) * p_name_s);
    if (NULL == p_name) setup_error("", "", MEMORY_ERROR);

    return p_name;
}

static char *parse_property_value(const struct cjlib_json *restrict src, const char *p_name)
{
    unsigned char curr_byte;
    int double_quotes_c = 0;

    size_t p_value_init_s = MEMORY_INIT_CHUNK;
    size_t p_value_s      = 0;

    char *p_value = (char *) malloc(sizeof(char) * p_value_init_s);
    if (NULL == p_value) {
        setup_error(p_name, "", MEMORY_ERROR);
        return NULL;
    }

    bool is_string  = false;
    bool is_object  = false;
    bool is_array   = false;
    bool type_found = false;

    do {
        curr_byte = (unsigned char) fgetc(src->c_fp);
        if (feof(src->c_fp)) {
            p_value[p_value_s] = '\0';
            setup_error(p_name, p_value, INVALID_PROPERTY);
            free(p_value);
            return NULL;
        }
        if (WHITE_SPACE == curr_byte) continue; // Check for ' '

        if (DOUBLE_QUOTES == curr_byte && !type_found) is_string = true; // Check for "
        else if (CURLY_BRACKETS_OPEN  == curr_byte && !type_found) is_object  = true; // Check for {
        else if (SQUARE_BRACKETS_OPEN == curr_byte && !type_found) is_array   = true; // Check for [

        if (DOUBLE_QUOTES == curr_byte) ++double_quotes_c; // Check for "
        
        if ((double_quotes_c > 0 && !is_string) || (double_quotes_c > 2 && is_string)) {
            p_value[p_value_s] = '\0';
            setup_error(p_name, p_value, MISSING_COMMA);
            free(p_value);
            return NULL;
        }

        p_value[p_value_s++] = curr_byte;
        if (p_value_s == p_value_init_s) {
            p_value_init_s += MEMORY_INIT_CHUNK;
            p_value = (char *) realloc(p_value, sizeof(char) * p_value_init_s);
            if (NULL == p_value) {
                setup_error(p_name, "", MEMORY_ERROR);
                return NULL;
            }
        }

        if (is_object || is_array) break;
        if (double_quotes_c < 2 && is_string && (COMMMA == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte)) {
            p_value[p_value_s] = '\0';
            setup_error(p_name, p_value, INCOMPLETE_DOUBLE_QUOTES);
            free(p_value);
            return NULL;
        } 
        if (COMMMA == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte) break; // Check for ,
    } while (1);

    if (is_object || is_array) {
        p_value[p_value_s] = '\0';
    } else {
        p_value[p_value_s - 1] = '\0'; // -1 stands for: Do not include the comma
    }

    p_value = (char *) realloc(p_value, sizeof(char) * p_value_s);
    if (NULL == p_value) setup_error(p_name, "", MEMORY_ERROR);

    return p_value;
}

int cjlib_json_read(struct cjlib_json *restrict dst)
{
    char *p_name = parse_property_name((const struct cjlib_json *) dst);

    if (NULL == p_name) printf("Failed to parse the name\n");
    else printf("%s\n", p_name);


    char *p_value = parse_property_value((const struct cjlib_json *) dst, p_name);
    if (*p_value == CURLY_BRACKETS_OPEN) printf("ITS OBJECT\n");

    if (NULL == p_value) printf("Failed to parse the value\n");
    else printf("%s\n", p_value);

    free(p_name);
    free(p_value);

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
