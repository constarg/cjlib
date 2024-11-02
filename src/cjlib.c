// @file: cjlib.c

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "cjlib.h"
#include "cjlib_error.h"
#include "cjlib_dictionary.h"
#include "cjlib_stack.h"

#define DOUBLE_QUOTES         (0x22) // ASCII representative of "
#define CURLY_BRACKETS_OPEN   (0x7B) // ASCII representative of {
#define SQUARE_BRACKETS_OPEN  (0x5B) // ASCII representative of [
#define CURLY_BRACKETS_CLOSE  (0x7D) // ASCII representative of }
#define SQUARE_BRACKETS_CLOSE (0x5D) // ASCII representative of ]
#define COMMMA                (0x2C) // ASCII representative of ,
#define WHITE_SPACE           (0x20) // ASCII representative of ' '
#define SEPERATOR             (0x3A) // ASCII representative of :

#define NEW_LINE              (0x0A) // ASCII representative of \n

#define MEMORY_INIT_CHUNK     (0x3C) // Hex representative of 60.
#define EXP_DOUBLE_QUOTES     (0x02) // Expected double quotes.

#define ROOT_PROPERTY_NAME    ("") // A name used to represent the beginning of the JSON. !NO OTHER PROPERTY MUST OBTAIN THIS NAME EXCEPT ROOT~



// Indicated whether the currently incomplete attribute is an object.
#define BUILDING_OBJECT(COMP) (CURLY_BRACKETS_CLOSE == COMP)
#define BUILDING_ARRAY(COMP) (SQUARE_BRACKETS_CLOSE == COMP)

// Receives a pointer value which are pointed to a string that may be an object or an array.
#define P_VALUE_BEGIN_OBJECT(VALUE_PTR) (*VALUE_PTR == CURLY_BRACKETS_OPEN)
#define P_VALUE_BEGIN_ARRAY(VALUE_PTR)  (*VALUE_PTR == SQUARE_BRACKETS_OPEN)

struct incomplete_property
{
    enum cjlib_json_datatypes i_type; // The type of the incomplete data (either array or object).
    char *i_name; // The name of the property that holds the incomplete data (in case of root object is WHITE_SPACE).
    union
    {
        cjlib_json_object *object; // The incomplete data is an object.
        cjlib_json_array *array;   // The incomplete data is an array.
    } i_data;
};

static void incomplete_property_init(struct incomplete_property *src)
{
    (void) memset(src, 0x0, sizeof(struct incomplete_property));
}

int cjlib_json_object_set
(cjlib_json_object *src, const char *restrict key,
 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype)
{
    value->c_datatype = datatype;
    if (-1 == cjlib_dict_insert(value, &src, key)) return -1;
    return 0;
}

int cjlib_json_object_get
(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src,
 const char *restrict key)
{
    if (NULL == dst) return -1;

    struct cjlib_json_data tmp;
    cjlib_json_data_init(&tmp);

    if (-1 == cjlib_dict_search(&tmp, src, key)) return -1;

    (void) memcpy(dst, &tmp, sizeof(struct cjlib_json_data));
    return 0;
}

int cjlib_json_object_remove
(struct cjlib_json_data *restrict dst, cjlib_json_object *src,
 const char *restrict key)
{
    // dst == NULL, then you can skip the return value.
    if (NULL == dst) goto perform_deletion;

    if (-1 == cjlib_json_object_get(dst, src, key)) return -1;

perform_deletion:
    if (-1 == cjlib_dict_remove(&src, key)) return -1;

    return 0;
}

int cjlib_json_open
(struct cjlib_json *restrict dst, const char *restrict json_path,
 const char *restrict modes)
{
    FILE *fp = fopen(json_path, modes);
    if (NULL == fp) return -1;
    if (-1 == cjlib_json_error_init()) return -1;

    dst->c_fp = fp;
    cjlib_dict_init(dst->c_dict);
    return 0;
}

void cjlib_json_close(struct cjlib_json *restrict src)
{
    cjlib_json_destroy(src);
    fclose(src->c_fp);
    (void) memset(src, 0x0, sizeof(struct cjlib_json));
}

static CJLIB_ALWAYS_INLINE bool is_number(const char *restrict src)
{
    while (*src) {
        if (isdigit(*src++) == 0) return false;
    }
    return true;
}

static CJLIB_ALWAYS_INLINE char *trim_double_quotes(const char *src)
{
    char *tmp    = strdup(src);
    size_t tmp_s = strlen(tmp);
    if (NULL == tmp) return NULL;

    (void) memmove(tmp, tmp + 1, tmp_s);
    tmp[tmp_s - 2] = '\0';

    return (char *) realloc(tmp, sizeof(char) * (tmp_s - 1));
}

static inline int type_decoder(struct cjlib_json_data *restrict dst, const char *p_name, const char *p_value)
{
    char *property_value = strdup(p_value);
    if (NULL == property_value) return -1;

    size_t property_len = strlen(property_value);

    enum cjlib_json_datatypes value_type;
    union cjlib_json_data_disting value;
    struct cjlib_json_data property;
    enum cjlib_json_error_types err_code;

    // build the data that is going to be inserted in the json.
    // The following if is required to find the problem in which a commma or a close bracket appeared on the end of a string.
    if (CURLY_BRACKETS_CLOSE == property_value[property_len - 1] || COMMMA == property_value[property_len - 1]
        || SQUARE_BRACKETS_CLOSE == property_value[property_len - 1]) {
        property_value[property_len - 1] = '\0';
        --property_len;
    }

    if (DOUBLE_QUOTES == property_value[0] && DOUBLE_QUOTES == property_value[property_len - 1]) {
        // Check for "
        value_type  = CJLIB_STRING;
        value.c_str = trim_double_quotes(property_value);
    } else if (!strcmp(property_value, "true") || !strcmp(property_value, "false")) {
        value_type      = CJLIB_BOOLEAN;
        value.c_boolean = (!strcmp(property_value, "true")) ? true : false;
    } else if (!strcmp(property_value, "null")) {
        value_type   = CJLIB_NULL;
        value.c_null = NULL;
    } else if (is_number(property_value)) {
        value_type  = CJLIB_NUMBER;
        value.c_num = strtol(property_value, NULL, 10);
    } else {
        err_code = INVALID_TYPE;
        goto type_decoder_err;
        return -1;
    }

    if (value_type == CJLIB_NUMBER && (LONG_MAX == value.c_num || LONG_MIN == value.c_num)) {
        err_code = INVALID_NUMBER;
        goto type_decoder_err;
        free(property_value);
        return -1;
    }
    property.c_datatype = value_type;
    property.c_value    = value;

    (void) memcpy(dst, &property, sizeof(struct cjlib_json_data));
    free(property_value);
    return 0;

type_decoder_err:
    if (NULL == p_name) cjlib_setup_error("", p_value, err_code);
    else cjlib_setup_error(p_name, p_value, err_code);

    return -1;
}

static char *parse_property_name(const struct cjlib_json *restrict src)
{
    unsigned char curr_byte;
    int double_quotes_c  = 0;
    bool found_seperator = false;

    size_t p_name_init_s = MEMORY_INIT_CHUNK;
    size_t p_name_s      = 0;

    char *p_name = (char *) malloc(sizeof(char) * p_name_init_s);
    if (NULL == p_name) {
        cjlib_setup_error("", "", MEMORY_ERROR);
        return NULL;
    }

    do {
        curr_byte = (unsigned char) fgetc(src->c_fp);
        if (feof(src->c_fp)) {
            p_name[p_name_s] = '\0';
            cjlib_setup_error(p_name, "", INVALID_PROPERTY);
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
                    cjlib_setup_error("", "", MEMORY_ERROR);
                    return NULL;
                }
            }
        }
        // How many double quotes are expected.
        if (EXP_DOUBLE_QUOTES == double_quotes_c && found_seperator) break;
        if (found_seperator) {
            p_name[p_name_s] = '\0';
            cjlib_setup_error(p_name, "", MISSING_SEPERATOR);
            free(p_name);
            return NULL;
        }
    } while (1);
    p_name[p_name_s - 1] = '\0'; // -1, to not include the seperator.

    p_name = (char *) realloc(p_name, sizeof(char) * p_name_s);
    if (NULL == p_name) cjlib_setup_error("", "", MEMORY_ERROR);

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
        cjlib_setup_error(p_name, "", MEMORY_ERROR);
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
            cjlib_setup_error(p_name, p_value, INVALID_PROPERTY);
            free(p_value);
            return NULL;
        }
        if (WHITE_SPACE == curr_byte) continue; // Check for ' '
        if (NEW_LINE == curr_byte) continue;    // Check for \n

        if (DOUBLE_QUOTES == curr_byte && !type_found) is_string = true;            // Check for "
        else if (CURLY_BRACKETS_OPEN == curr_byte && !type_found) is_object = true; // Check for {
        else if (SQUARE_BRACKETS_OPEN == curr_byte && !type_found) is_array = true; // Check for [

        if (DOUBLE_QUOTES == curr_byte) ++double_quotes_c; // Check for "

        if ((double_quotes_c > 0 && !is_string) || (double_quotes_c > 2 && is_string)) {
            p_value[p_value_s] = '\0';
            cjlib_setup_error(p_name, p_value, MISSING_COMMA);
            free(p_value);
            return NULL;
        }

        p_value[p_value_s++] = curr_byte;
        if (p_value_s == p_value_init_s) {
            p_value_init_s += MEMORY_INIT_CHUNK;
            p_value = (char *) realloc(p_value, sizeof(char) * p_value_init_s);
            if (NULL == p_value) {
                cjlib_setup_error(p_name, "", MEMORY_ERROR);
                return NULL;
            }
        }

        if (is_object || is_array) break;
        if (double_quotes_c < 2 && is_string && (COMMMA == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte)) {
            p_value[p_value_s] = '\0';
            cjlib_setup_error(p_name, p_value, INCOMPLETE_DOUBLE_QUOTES);
            free(p_value);
            return NULL;
        }
        // Check for ,
        if (COMMMA == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte || SQUARE_BRACKETS_CLOSE == curr_byte) break;
    } while (1);

    p_value[p_value_s] = '\0';

    p_value = (char *) realloc(p_value, sizeof(char) * (p_value_s + 1));
    if (NULL == p_value) cjlib_setup_error(p_name, "", MEMORY_ERROR);

    return p_value;
}

static int configure_common
(struct incomplete_property *restrict src, const char *p_name,
 void **restrict data, enum cjlib_json_datatypes p_type)
{
    src->i_name = strdup(p_name);
    if (NULL == src->i_name) return -1;

    switch (p_type) {
        case CJLIB_OBJECT:
            *((cjlib_json_object **) data) = cjlib_json_make_object();
            break;
        case CJLIB_ARRAY:
            *((cjlib_json_array **) data) = cjlib_json_make_array();
            break;
        default:
            break;
    }
    if (NULL == *data) return -1;

    return 0;
}

static CJLIB_ALWAYS_INLINE int configure_nested_object(struct incomplete_property *restrict src, const char *p_name)
{
    src->i_type = CJLIB_OBJECT;
    return configure_common(src, p_name, (void *) &src->i_data.object, CJLIB_OBJECT);
}

static CJLIB_ALWAYS_INLINE int configure_array(struct incomplete_property *restrict src, const char *p_name)
{
    src->i_type = CJLIB_ARRAY;
    return configure_common(src, p_name, (void *) &src->i_data.array, CJLIB_ARRAY);
}

static void *restore_common
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    void *parent_data = NULL;
    struct cjlib_json_data comp_data;

    char *p_name_trimmed = trim_double_quotes(comp->i_name);
    void *i_data         = NULL;
    void **c_value       = NULL;
    size_t i_data_s      = 0;

    if (CJLIB_OBJECT == comp->i_type) {
        i_data   = comp->i_data.object;
        i_data_s = sizeof(cjlib_json_object);
        c_value  = (void **) &comp_data.c_value.c_obj;
    } else {
        i_data   = comp->i_data.array;
        i_data_s = sizeof(cjlib_json_array);
        c_value  = (void **) &comp_data.c_value.c_arr;
    }

    comp_data.c_datatype = comp->i_type;
    *c_value = (CJLIB_OBJECT == comp->i_type) ? (void *) cjlib_json_make_object() : (void *) cjlib_json_make_array();
    if (NULL == c_value) return NULL;

    (void) memcpy(*c_value, i_data, i_data_s);

    if (CJLIB_OBJECT == parent->i_type) {
        parent_data = parent->i_data.object;
        if (-1 == cjlib_dict_insert(&comp_data, (cjlib_json_object **) &parent_data, p_name_trimmed)) return NULL;
    } else {
        parent_data = parent->i_data.array;
        if (-1 == cjlib_json_array_append((cjlib_json_array *) parent_data, &comp_data)) return NULL;
    }

    free(p_name_trimmed);
    return parent_data;
}

static CJLIB_ALWAYS_INLINE cjlib_json_object *restore_obj_from_nested_obj
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    return (cjlib_json_object *) restore_common(comp, parent);
}

static CJLIB_ALWAYS_INLINE cjlib_json_object *restore_obj_from_array
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    return (cjlib_json_object *) restore_common(comp, parent);
}

static cjlib_json_object *actions_before_obj_restore
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    switch (comp->i_type) {
        case CJLIB_OBJECT:
            return restore_obj_from_nested_obj(comp, parent);
        case CJLIB_ARRAY:
            return restore_obj_from_array(comp, parent);
        default:
            return NULL;
    }
}

static CJLIB_ALWAYS_INLINE int restore_arr_from_object
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    if (NULL == restore_common(comp, parent)) return -1;
    return 0;
}

static CJLIB_ALWAYS_INLINE int restore_arr_from_nested_arr
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    if (NULL == restore_common(comp, parent)) return -1;
    return 0;
}

static CJLIB_ALWAYS_INLINE int actions_before_array_restore
(const struct incomplete_property *restrict comp, const struct incomplete_property *restrict parent)
{
    switch (comp->i_type) {
        case CJLIB_OBJECT:
            return restore_arr_from_object(comp, parent);
        case CJLIB_ARRAY:
            return restore_arr_from_nested_arr(comp, parent);
        default:
            return -1;
    }
}

static CJLIB_ALWAYS_INLINE int reached_end_of_json(const struct cjlib_json *restrict src)
{
    // Get the current position in the file.
    long restore_pos; // The position to return.
    unsigned char curr_byte;

    restore_pos = ftell(src->c_fp);
    if (-1 == restore_pos) return -1;

    do {
        curr_byte = fgetc(src->c_fp);
    } while (WHITE_SPACE == curr_byte || NEW_LINE == curr_byte);

    if (-1 == fseek(src->c_fp, restore_pos, SEEK_SET)) return -1; // Reset the file offset.

    if (CURLY_BRACKETS_CLOSE == curr_byte) return true;

    return false;
}

int cjlib_json_read(struct cjlib_json *restrict dst)
{
    /**
     *  Algorithm description for objects:
     *  1. Create an incomplete object.
     *  2. Start filling the imcomplete object with elements until the close brackets '}' symbol show up.
     *  3. if filling process a open bracket show up '{', then put the currently incomplete object in a stacK and go to step 1.
     */

    /**
     *  Algorithm description for arrays:
     *  1. If on filling process of the incomplete object an open square bracket show up '[' create an incomplete array.
     *  2. Start the filling process as it is an object, except that there are no names coresponded for each value.
     *  3. IF an object show up, indicating by open curly bracket '{', when create a new imcomplete object.
     */

    struct cjlib_stack incomplate_data_stc;
    struct incomplete_property curr_incomplete_data;
    struct incomplete_property tmp_data;
    struct cjlib_json_data complete_data;

    // A byte which indicate that the currently incomplete data are complete.
    char compl_indicator = CURLY_BRACKETS_CLOSE;
    char *p_value        = NULL;
    char *p_name         = NULL;
    char *p_name_trimmed = NULL;

    int reach_end;

    cjlib_stack_init(&incomplate_data_stc);
    incomplete_property_init(&tmp_data);
    incomplete_property_init(&curr_incomplete_data);

    curr_incomplete_data.i_name = strdup(ROOT_PROPERTY_NAME); // cause is the root object. (TODO - free this.)
    if (NULL == curr_incomplete_data.i_name) goto read_err;

    curr_incomplete_data.i_data.object = dst->c_dict;
    curr_incomplete_data.i_type        = CJLIB_OBJECT;

    // Push the first incomplete object into the stack.
    if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property),
                               &incomplate_data_stc))
        goto read_err;

    while (!cjlib_stack_is_empty(&incomplate_data_stc)) {
        if (BUILDING_OBJECT(compl_indicator)) {
            // Building an object?
            p_name = parse_property_name((const struct cjlib_json *) dst);
            if (NULL == p_name) printf("Failed to parse the name\n"); // TODO - replace with the actual error
        }
        p_value = parse_property_value((const struct cjlib_json *) dst, p_name);
        if (NULL == p_value) printf("Failed to parse the value of the property\n");
        // TODO - replace with the actual error.

        if (P_VALUE_BEGIN_OBJECT(p_value)) {
            if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property),
                                       &incomplate_data_stc))
                goto read_err;
            if (CJLIB_ARRAY == curr_incomplete_data.i_type) p_name = strdup("");
            if (NULL == p_name) goto read_err;
            if (-1 == configure_nested_object(&curr_incomplete_data, p_name)) goto read_err;

            compl_indicator = CURLY_BRACKETS_CLOSE;
            free(p_name);
            free(p_value);
            p_name  = NULL;
            p_value = NULL;
            continue;
        } else if (P_VALUE_BEGIN_ARRAY(p_value)) {
            //if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property), &incomplate_data_stc)) goto read_err;

            if (-1 == configure_array(&curr_incomplete_data, p_name)) goto read_err;
            free(p_name);
            free(p_value);
            p_name          = NULL;
            p_value         = NULL;
            compl_indicator = SQUARE_BRACKETS_CLOSE;
            continue;
        }

        if (-1 == type_decoder(&complete_data, p_name, p_value)) goto read_err;

        if (BUILDING_OBJECT(compl_indicator)) {
            p_name_trimmed = trim_double_quotes(p_name);
            if (-1 == cjlib_dict_insert(&complete_data, &curr_incomplete_data.i_data.object,
                                        p_name_trimmed))
                goto read_err;
        } else {
            // TODO - DO NOT APPEND THE CLOSE SQUARE BRACKET SYMBOL.
            if (-1 == cjlib_json_array_append(curr_incomplete_data.i_data.array, &complete_data)) goto read_err;
        }

        if (compl_indicator == p_value[strlen(p_value) - 1]) {
            if (-1 == cjlib_stack_pop((void *) &tmp_data, sizeof(struct incomplete_property),
                                      &incomplate_data_stc))
                goto read_err;
            complete_data.c_datatype = tmp_data.i_type;

            if (memcmp(&tmp_data, &curr_incomplete_data, sizeof(struct incomplete_property)) != 0) {
                switch (complete_data.c_datatype) {
                    case CJLIB_OBJECT:
                        // Update the root of the AVL tree.
                        tmp_data.i_data.object = actions_before_obj_restore(&curr_incomplete_data, &tmp_data);
                        free(curr_incomplete_data.i_name); // strdup, func -> configure_common
                        free(curr_incomplete_data.i_data.object);
                        (void) memcpy(&curr_incomplete_data, &tmp_data, sizeof(struct incomplete_property));
                        compl_indicator = CURLY_BRACKETS_CLOSE;
                        break;
                    case CJLIB_ARRAY:
                        if (-1 == actions_before_array_restore(&curr_incomplete_data,
                                                               &tmp_data))
                            goto read_err;
                        free(curr_incomplete_data.i_data.array);
                        (void) memcpy(&curr_incomplete_data, &tmp_data, sizeof(struct incomplete_property));
                        compl_indicator = SQUARE_BRACKETS_CLOSE;
                        break;
                    default:
                        break;
                }
            } else {
                if (!strcmp(tmp_data.i_name, ROOT_PROPERTY_NAME)) {
                    free(tmp_data.i_name);
                    tmp_data.i_name = NULL;
                    goto read_cleanup; // If this statement occur, then skip the end of file verification
                }
            }

            reach_end = reached_end_of_json(dst);
            if (-1 == reach_end) goto read_err;

            if (!strcmp(tmp_data.i_name, ROOT_PROPERTY_NAME) && !reach_end) {
                // This statement must be applied to ensure that all elements of the JSON file will be parsed, after an object closure.
                //if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property), &incomplate_data_stc)) goto read_err;
            }
        }

read_cleanup:
        free(p_name);
        free(p_value);
        free(p_name_trimmed);
        p_name         = NULL;
        p_value        = NULL;
        p_name_trimmed = NULL;
    }
    free(tmp_data.i_name);

    // Update the AVL tree.
    dst->c_dict = curr_incomplete_data.i_data.object; // Is no longer incomplete.

    return 0;

read_err:
    free(p_name);
    free(p_value);
    free(p_name_trimmed);

    return -1;
}

char *cjlib_json_object_stringtify(const cjlib_json_object *restrict src)
{
    return NULL;
}

int cjlib_json_dump(const struct cjlib_json *restrict src)
{
    // 1. Translate the dictionary from the memory to json format.
    // 2. Write the resulted string in the json file.
    return 0;
}
