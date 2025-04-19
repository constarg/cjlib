/* File: cjlib.c
 ************************************************************************
 * Copyright (C) 2024-2025 Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include "cjlib.h"
#include "cjlib_error.h"
#include "cjlib_dictionary.h"
#include "cjlib_stack.h"
#include "cjlib_list.h"
#include "cjlib_queue.h"

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

/**
 * Used to describe an incomplete object or array in the process
 * of stringtifing the object/array stored in the RAM
 */
struct incomplete_property_str
{
    char *i_key; // The key of the current incomplete element.
    char *i_state; // How much of the JSON stringtifying process is done for the incomplete data.
    bool set_comma; // Determines whether to set commad when completing an object/array.
    enum cjlib_json_datatypes i_type; // The type of the incompelte object (object or array)
    struct cjlib_queue *i_pending_data_q; // The data that must be stored in the incomplete object (but not expanded yet).
    union {
        cjlib_json_object *object; // The incomplete data is an object.
        cjlib_json_array *array;   // The incompelte data is an array.
    } i_data;
};

static inline void incomplete_property_init(struct incomplete_property *src)
{
    (void) memset(src, 0x0, sizeof(struct incomplete_property));
}

static inline void incomplete_property_str_init(struct incomplete_property_str *src) 
{
    (void) memset(src, 0x0, sizeof(struct incomplete_property_str));
}

/**
 * Recieve the current active state of an object and expanded using
 * by storing the @data on the end of the @state.
 *
 * @param state The state of the incomplete data untill now.
 * @param data The data to add to the state.
 * @return The new state on success, otherwise NULL.
 */
static inline char *incomplete_property_str_expand_state
(const char *state, const char *data, const char *key)
{
    size_t key_size = (NULL == key)? 0 : strlen(key);
    size_t colon_len = 1;
    char *new_state = (char *) malloc(strlen(state) + strlen(data) + key_size + colon_len + 1);
    if (NULL == new_state) return NULL;
    
    if (NULL == key) {
        (void) sprintf(new_state, "%s%s", state, data);
    } else {
        (void) sprintf(new_state, "%s%s:%s", state, key, data);
    }

    return new_state;
}

int cjlib_json_object_set
(cjlib_json_object **src, const char *restrict key,
 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype)
{
    struct cjlib_json_data dummy;

    // Remove the previous contents (if exists).
    (void) cjlib_json_object_remove(&dummy, src, key);
    cjlib_json_data_destroy(&dummy);

    // (change/set) the record.
    value->c_datatype = datatype;
    if (-1 == cjlib_dict_insert(value, src, key)) return -1;
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
(struct cjlib_json_data *restrict dst, cjlib_json_object **src,
 const char *restrict key)
{
    // dst == NULL, then you can skip the return value.
    if (NULL == dst) goto perform_deletion;

    if (-1 == cjlib_json_object_get(dst, *src, key)) return -1;

perform_deletion:
    if (-1 == cjlib_dict_remove(src, key)) return -1;

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

    dst->c_path = strdup(json_path);
    return 0;
}

void cjlib_json_close(struct cjlib_json *restrict src)
{
    cjlib_json_destroy(src);
    fclose(src->c_fp);
    (void) memset(src, 0x0, sizeof(struct cjlib_json));
 
    cjlib_json_error_destroy();
}

static CJLIB_ALWAYS_INLINE bool is_number(const char *restrict src)
{
    while (*src) {
        if (isalpha(*src) || isblank(*src)) return false;
        src++;
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
        value.c_num = atof(property_value);
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
    long retreat_pos = ftell(src->c_fp); // In case of retread, restore the file position.
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

        if (WHITE_SPACE == curr_byte && 0 == double_quotes_c) continue; // Check for ' '
        if (WHITE_SPACE == curr_byte && EXP_DOUBLE_QUOTES == double_quotes_c) continue;

        if ((CURLY_BRACKETS_OPEN   == curr_byte  || 
             CURLY_BRACKETS_CLOSE  == curr_byte ||
             SQUARE_BRACKETS_OPEN  == curr_byte ||
             SQUARE_BRACKETS_CLOSE == curr_byte) && 0 == double_quotes_c) {
            // Retreat!!, THIS is a name (not always an error)
            fseek(src->c_fp, retreat_pos, SEEK_SET);
            free(p_name);
            return strdup("");
        }

        // Check fof :
        if (SEPERATOR == curr_byte) found_seperator = true;

        if (EXP_DOUBLE_QUOTES == double_quotes_c && !found_seperator) {
            p_name[p_name_s] = '\0';
            cjlib_setup_error(p_name, "", MISSING_SEPERATOR);
            free(p_name);
            return NULL;
        }

        // Check for "
        if (DOUBLE_QUOTES == curr_byte) ++double_quotes_c;

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
            cjlib_setup_error(p_name, "", INCOMPLETE_DOUBLE_QUOTES);
            free(p_name);
            return NULL;
        }
    } while (1);
    p_name[p_name_s - 1] = '\0'; // -1, to not include the seperator.

    p_name = (char *) realloc(p_name, sizeof(char) * p_name_s);
    if (NULL == p_name) cjlib_setup_error("", "", MEMORY_ERROR);

    return p_name;
}

static inline int next_is_end_of_file(FILE *restrict file_ptr)
{
    long restore_pos = ftell(file_ptr);
    unsigned char curr_byte;

    if (-1 == restore_pos) return -1;

    curr_byte = fgetc(file_ptr);
    (void) curr_byte;

    if (-1 == fseek(file_ptr, restore_pos, SEEK_SET)) return -1; // Reset the file offset.

    if (feof(file_ptr)) return true;

    return false;
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
        if (WHITE_SPACE == curr_byte && !is_string) continue; // Check for ' '
        if (WHITE_SPACE == curr_byte && is_string && EXP_DOUBLE_QUOTES == double_quotes_c) continue;

        if (NEW_LINE == curr_byte) continue; // Check for \n

        if (DOUBLE_QUOTES == curr_byte && !type_found) is_string = type_found = true;            // Check for "
        else if (CURLY_BRACKETS_OPEN == curr_byte && !type_found) is_object = type_found = true; // Check for {
        else if (SQUARE_BRACKETS_OPEN == curr_byte && !type_found) is_array = type_found = true; // Check for [

        if (DOUBLE_QUOTES == curr_byte) ++double_quotes_c; // Check for "

        if ((double_quotes_c > 0 && !is_string) || (double_quotes_c > EXP_DOUBLE_QUOTES && is_string)) {
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

        if ((is_object || is_array) && (!is_string || double_quotes_c == EXP_DOUBLE_QUOTES)) break;
        if (double_quotes_c < EXP_DOUBLE_QUOTES && is_string && (COMMMA == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte) 
            && next_is_end_of_file(src->c_fp)) {
            p_value[p_value_s] = '\0';
            cjlib_setup_error(p_name, p_value, INCOMPLETE_DOUBLE_QUOTES);
            free(p_value);
            return NULL;
        }

        if ((COMMMA == curr_byte || CURLY_BRACKETS_CLOSE == curr_byte || SQUARE_BRACKETS_CLOSE == curr_byte) &&
            (!is_string || double_quotes_c == EXP_DOUBLE_QUOTES)) break;
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

    char *p_name_trimmed = (!strcmp(ROOT_PROPERTY_NAME, comp->i_name))? strdup(comp->i_name):trim_double_quotes(comp->i_name);
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

    if (CURLY_BRACKETS_CLOSE == curr_byte || feof(src->c_fp)) return true;

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

    curr_incomplete_data = (struct incomplete_property) {
        .i_name = strdup(ROOT_PROPERTY_NAME), // cause is the root object
        .i_type = CJLIB_OBJECT,
        .i_data.object = dst->c_dict
    };

    if (NULL == curr_incomplete_data.i_name) goto read_err;

    // Push the first incomplete object into the stack.
    if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property),
                               &incomplate_data_stc))
        goto read_err;

    while (!cjlib_stack_is_empty(&incomplate_data_stc)) {
        if (BUILDING_OBJECT(compl_indicator)) {
            // Building an object?
            p_name = parse_property_name((const struct cjlib_json *) dst);
            if (NULL == p_name) goto read_err;
        }
        p_value = parse_property_value((const struct cjlib_json *) dst, p_name);
        if (NULL == p_value) goto read_err;

        // If the current data is only a 'comma', nothing else, then ignore it.
        if (!strcmp(p_value, ",")) goto read_cleanup;
        
        if (BUILDING_OBJECT(compl_indicator)) {
            if (!strcmp(ROOT_PROPERTY_NAME, p_name) && 
                !strcmp(ROOT_PROPERTY_NAME, curr_incomplete_data.i_name)) goto read_cleanup;
        }

        if (P_VALUE_BEGIN_OBJECT(p_value)) {
            if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property),
                                       &incomplate_data_stc))
                goto read_err;
            if (CJLIB_ARRAY == curr_incomplete_data.i_type) p_name = strdup(curr_incomplete_data.i_name);
            if (NULL == p_name) goto read_err;
            if (-1 == configure_nested_object(&curr_incomplete_data, p_name)) goto read_err;

            compl_indicator = CURLY_BRACKETS_CLOSE;
            goto read_cleanup;
        } else if (P_VALUE_BEGIN_ARRAY(p_value)) {
            if (-1 == cjlib_stack_push((void *) &curr_incomplete_data, sizeof(struct incomplete_property),
                                       &incomplate_data_stc))
                goto read_err;

            if (-1 == configure_array(&curr_incomplete_data, p_name)) goto read_err;
            compl_indicator = SQUARE_BRACKETS_CLOSE;
            goto read_cleanup;
        }

        if (-1 == type_decoder(&complete_data, p_name, p_value)) goto read_err;

        if (BUILDING_OBJECT(compl_indicator) && CURLY_BRACKETS_CLOSE != p_value[0]) {
            p_name_trimmed = trim_double_quotes(p_name);
            if (-1 == cjlib_dict_insert(&complete_data, &curr_incomplete_data.i_data.object,
                                        p_name_trimmed))
                goto read_err;
        } else if (BUILDING_ARRAY(compl_indicator) && SQUARE_BRACKETS_CLOSE != p_value[0]) {
            if (-1 == cjlib_json_array_append(curr_incomplete_data.i_data.array, &complete_data)) goto read_err;
        }

        if (compl_indicator == p_value[strlen(p_value) - 1]) {
            if (-1 == cjlib_stack_pop((void *) &tmp_data, sizeof(struct incomplete_property),
                                      &incomplate_data_stc))
                goto read_err;
            complete_data.c_datatype = tmp_data.i_type;

            if (!strcmp(tmp_data.i_name, ROOT_PROPERTY_NAME) && 
                !strcmp(curr_incomplete_data.i_name, ROOT_PROPERTY_NAME)) {
                    tmp_data = curr_incomplete_data;
            }

            if (memcmp(&tmp_data, &curr_incomplete_data, sizeof(struct incomplete_property)) != 0) {
                switch (complete_data.c_datatype) {
                    case CJLIB_OBJECT:
                        // Update the root of the AVL tree.
                        tmp_data.i_data.object = actions_before_obj_restore(&curr_incomplete_data, &tmp_data);
                        free(curr_incomplete_data.i_name);
                        (CJLIB_OBJECT == curr_incomplete_data.i_type) ?
                            free(curr_incomplete_data.i_data.object) :
                            free(curr_incomplete_data.i_data.array);

                        (void) memcpy(&curr_incomplete_data, &tmp_data, sizeof(struct incomplete_property));
                        compl_indicator = CURLY_BRACKETS_CLOSE;
                        break;
                    case CJLIB_ARRAY:
                        if (-1 == actions_before_array_restore(&curr_incomplete_data,
                                                               &tmp_data))
                            goto read_err;
                        free(curr_incomplete_data.i_name);
                        (CJLIB_OBJECT == curr_incomplete_data.i_type) ?
                            free(curr_incomplete_data.i_data.object) :
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

            if (!strcmp(tmp_data.i_name, ROOT_PROPERTY_NAME) && reach_end) {
                if (-1 == cjlib_stack_pop((void *) &tmp_data, sizeof(struct incomplete_property),
                                          &incomplate_data_stc))
                    goto read_err;
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

/**
 * Enclose a state which represent a complete JSON entry with its 
 * respective opening and closing symbol. For example is the 
 * completed state were an object, then the opening symbol is { (curly brackets open)
 * and the closing symbol is } (curly bracktes close)
 *
 * @param entry_state The state of the complete entry.
 * @param opening_symbol The opening symbol of the entry.
 * @param closing_symbol The closing symbol of the entry.
 * @returns A string representing the opening_symbol entry closing_symbol on success. 
 * Otherwise NULL.
 */
static inline char *wrap_complete_entry(const char *entry_state, char opening_symbol, 
                                        char closing_symbol, bool set_comma) 
{
    size_t additional_size = 3; // The size of {} or [] + comma.
    size_t wrapped_size = strlen(entry_state) + additional_size;

    char *wrapped_state = (char *) malloc(wrapped_size + 1);
    if (NULL == wrapped_state) return NULL;

    if (set_comma) {
        (void) sprintf(wrapped_state, "%c%s%c,", opening_symbol, entry_state, closing_symbol); 
    } else {
        (void) sprintf(wrapped_state, "%c%s%c", opening_symbol, entry_state, closing_symbol); 
    }

    return wrapped_state;
}

/**
 * Take the data of the current JSON field and its respective key as an argument 
 * and produce a string that is in format KEY : DATA, 
 *
 * @param src The data of the field.
 * @param set_comma Determines whether to insert a comma or not.
 * @return A string representing an entry of the JSON file (Format KEY : DATA,) -> (KEY COLON DATA COMMA) in success, 
 * otherwise NULL is returned.
 */
static char *simple_key_value_paired_stringtify
(const struct cjlib_json_data *restrict src, bool set_comma)
{
    const size_t comma_len = 1;
    const size_t colon_len = 1;
    const size_t boolean_true_len  = 4;
    const size_t boolean_false_len = 5;
    const size_t null_len = 4;
    const size_t double_quotes_len = 2;
    size_t digit_num = 0;

    char *result = NULL;
    switch (src->c_datatype) {
        case CJLIB_STRING:
            // LEN(KEY) + LEN(:) + LEN(VALUE) + LEN(,) + LEN("\0")
            result = (char *) malloc(strlen(src->c_value.c_str) + comma_len + colon_len + double_quotes_len + 1);
            if (NULL == result) return NULL;
            if (set_comma) {
                (void) sprintf(result, "\"%s\",", src->c_value.c_str);
            } else {
                (void) sprintf(result, "\"%s\"", src->c_value.c_str);
            }
            break;
        case CJLIB_NUMBER:
            digit_num = snprintf(NULL, 0, "%f", src->c_value.c_num);
            result = (char *) malloc(digit_num + comma_len + colon_len + 1);
            if (NULL == result) return NULL;

            if (set_comma) {
                (void) sprintf(result, "%f,", src->c_value.c_num);
            } else {
                (void) sprintf(result, "%f", src->c_value.c_num);
            }
            break;
        case CJLIB_BOOLEAN:
            if (src->c_value.c_boolean) {
                result = (char *) malloc(boolean_true_len + comma_len + colon_len + 1);
                if (NULL == result) return NULL;

                if (set_comma) {
                    (void) sprintf(result, "%s,", "true");
                } else {
                    (void) sprintf(result, "%s", "true");
                }
            } else {
                result = (char *) malloc(boolean_false_len + comma_len + colon_len + 1);
                if (NULL == result) return NULL;

                if (set_comma) {
                    (void) sprintf(result, "%s,", "false");
                } else {
                    (void) sprintf(result, "%s", "false");
                }
            }
            break;
        case CJLIB_NULL:
            result = (char *) malloc(null_len + comma_len + colon_len + 1);
            if (NULL == result) return NULL;

            if (set_comma) {
                (void) sprintf(result, "%s,", "null");
            } else {
                (void) sprintf(result, "%s", "null");
            }
            break;
        default:
            break;
    }

    return result;
}

/**
 * Switches to another incomplete object,i.e., when a nested array or object is
 * detected.
 *
 * @param dst A pointer that points to the location to store the switched data.
 * @param type The type of the new data.
 * @param entry The incomplete object/array.
 * @param entry_key The key of the key:pair combination for the incomplete object/array.
 */
static inline int switch_from_incomplete_obj_str_data
(struct incomplete_property_str *restrict dst, enum cjlib_json_datatypes type, 
 cjlib_json_object *entry)
{
    char opening_symbol_str = DOUBLE_QUOTES;
    char closing_symbol_str = DOUBLE_QUOTES;
    size_t key_wrapped_size = strlen(CJLIB_DICT_NODE_KEY(entry)) + 2;
    char *key_wrapped = (char *) malloc(key_wrapped_size + 1);
    if (NULL == key_wrapped) return -1;

    (void) sprintf(key_wrapped, "%c%s%c", opening_symbol_str, CJLIB_DICT_NODE_KEY(entry), 
                   closing_symbol_str);

    struct cjlib_json_data *examine_entry_data = CJLIB_DICT_NODE_DATA(entry);
    *dst = (struct incomplete_property_str) {
        .i_key            = key_wrapped,
        .set_comma        = true,
        .i_type           = type,
        .i_state          = strdup(""),
        .i_pending_data_q = (struct cjlib_queue *) malloc(sizeof(struct cjlib_queue))
    };

    if (NULL == dst->i_pending_data_q) return -1;

    cjlib_queue_init(dst->i_pending_data_q);

    if (CJLIB_OBJECT == type) dst->i_data.object = entry;
    else dst->i_data.array = examine_entry_data->c_value.c_arr;

    return 0;
}

static inline int switch_from_incomplete_arr_str_data
(struct incomplete_property_str *restrict dst, enum cjlib_json_datatypes type, 
struct cjlib_json_data *entry_data, const char *key)
{
    char opening_symbol_str = DOUBLE_QUOTES;
    char closing_symbol_str = DOUBLE_QUOTES;
    size_t key_wrapped_size = strlen(key) + 2;
    char *key_wrapped = (char *) malloc(key_wrapped_size + 1);
    if (NULL == key_wrapped) return -1;

    (void) sprintf(key_wrapped, "%c%s%c", opening_symbol_str, key, 
                   closing_symbol_str);

    struct cjlib_json_data *examine_entry_data = entry_data;
    *dst = (struct incomplete_property_str) {
        .i_key            = key_wrapped,
        .set_comma        = true,
        .i_type           = type,
        .i_state          = strdup(""),
        .i_pending_data_q = (struct cjlib_queue *) malloc(sizeof(struct cjlib_queue))
    };

    if (NULL == dst->i_pending_data_q) return -1;

    cjlib_queue_init(dst->i_pending_data_q);

    if (CJLIB_OBJECT == type) dst->i_data.object = examine_entry_data->c_value.c_obj;
    else dst->i_data.array = examine_entry_data->c_value.c_arr;

    return 0;
}
/**
 * Convert the provided @src list into a queue.
 *
 * The queue consists of pointers that point to the objects
 * in the list. (NO NEED FOR FREE FOR THOSE OBJECTS NEEDED).
 * This is done for optimization purposes (to allocate and free
 * two times the same data).
 *
 * @param dst A pointer to the queue in which the pointers are stored.
 * @param src A pointer to the list from which the paointers will be extracted.
 * @return 0 on success, otherwise -1.
 */
static inline int convert_list_to_queue(struct cjlib_queue *restrict dst, const struct cjlib_list *restrict src)
{
    struct cjlib_json_data *item; 

    CJLIB_LIST_FOR_EACH_PTR(item, src, struct cjlib_json_data) {
        // Store the pointers of each element in the list into the @dst
        if (-1 == cjlib_queue_enqeue((void *) &item, sizeof(struct cjlib_json_data *), dst)) {
            return -1;
        }
    }

    return 0;
}

const char *cjlib_json_object_stringtify(const cjlib_json_object *src)
{
    struct incomplete_property_str curr_incomp; // The currently expanding data.
    struct cjlib_stack incomplete_st;           // The stack of incomplete data.
    
    cjlib_dict_node_t *examine_entry;           // The current key:paired entry from the JSON in memory. (contains both the key and data)
    struct cjlib_json_data *examine_entry_data; // The data of the examined entry.

    char *tmp_state = NULL; // Used to control the memory (see default case in the switch below)
    char *tmp_key   = NULL; // Used to temporary store the key of the complete JSON object/array.
    char *value_str = NULL; // Used to temporary store the strintify data (see default case in the switch below)

    bool set_comma_tmp = true; // Helper, to maintain the information about whether to put comma or not on completion (see incomplete_str & on switch below.)

    char opening_symbol_obj = CURLY_BRACKETS_OPEN;
    char opening_symbol_arr = SQUARE_BRACKETS_OPEN;
    char closing_symbol_obj = CURLY_BRACKETS_CLOSE;
    char closing_symbol_arr = SQUARE_BRACKETS_CLOSE;

    char opening_symbol_str = DOUBLE_QUOTES;
    char closing_symbol_str = DOUBLE_QUOTES;
    
    char opening_symbol;
    char closing_symbol;

    // Initiazize the incomplete data.
    incomplete_property_str_init(&curr_incomp);
    cjlib_stack_init(&incomplete_st);

    if (-1 == switch_from_incomplete_obj_str_data(&curr_incomp, CJLIB_OBJECT, (cjlib_json_object *) src)) return NULL;

    if (-1 == cjlib_dict_preorder(curr_incomp.i_pending_data_q, curr_incomp.i_data.object)) return NULL;

    // Put a dummy entry in the stack to start the process.
    if (-1 == cjlib_stack_push((void *) &curr_incomp, sizeof(struct incomplete_property_str), 
                               &incomplete_st)) return NULL;

    while (!cjlib_stack_is_empty(&incomplete_st)) {
        if (-1 == cjlib_stack_pop((void *) &curr_incomp, sizeof(struct incomplete_property_str),
                                  &incomplete_st)) return NULL;

        // If the value_str is NULL, then, the nested while have not run yet.
        if (NULL != value_str) {
            tmp_state = curr_incomp.i_state;
            if (CJLIB_OBJECT == curr_incomp.i_type) {
                curr_incomp.i_state = incomplete_property_str_expand_state(curr_incomp.i_state, value_str, tmp_key);
            } else {
                curr_incomp.i_state = incomplete_property_str_expand_state(curr_incomp.i_state, value_str, NULL); 
            }

            free(tmp_key);
            free(value_str);
            free(tmp_state);
            tmp_state = NULL;
            value_str = NULL;
            tmp_key   = NULL;
        }

        while (!cjlib_queue_is_empty(curr_incomp.i_pending_data_q)) {
            // Get the entry to strintify.
            if (CJLIB_OBJECT == curr_incomp.i_type) {
                // The queue of an incomplete object consists of NODES of avl binary tree.
                cjlib_queue_deqeue((void *) &examine_entry, sizeof(cjlib_dict_node_t *), curr_incomp.i_pending_data_q);
                examine_entry_data = CJLIB_DICT_NODE_DATA(examine_entry);
            } else {
                // The queue of an incomplete array consists of NODES of a linked list.
                cjlib_queue_deqeue((void *) &examine_entry_data, sizeof(struct cjlib_json_data *), curr_incomp.i_pending_data_q);
            }

            switch (examine_entry_data->c_datatype) {
                case CJLIB_OBJECT:
                    if (cjlib_queue_is_empty(curr_incomp.i_pending_data_q)) set_comma_tmp = false;
                
                    cjlib_stack_push(&curr_incomp, sizeof(struct incomplete_property_str), 
                                     &incomplete_st);

                    if (CJLIB_ARRAY == curr_incomp.i_type) examine_entry = examine_entry_data->c_value.c_obj;
                    if (-1 == switch_from_incomplete_obj_str_data(&curr_incomp, CJLIB_OBJECT, examine_entry)) return NULL;

                    curr_incomp.set_comma = set_comma_tmp;
                    
                    if (-1 == cjlib_dict_preorder(curr_incomp.i_pending_data_q, curr_incomp.i_data.object)) return NULL;
                    break;
                case CJLIB_ARRAY:
                    if (cjlib_queue_is_empty(curr_incomp.i_pending_data_q)) set_comma_tmp = false;
                    cjlib_stack_push(&curr_incomp, sizeof(struct incomplete_property_str), &incomplete_st);

                    if (-1 == switch_from_incomplete_arr_str_data(&curr_incomp, CJLIB_ARRAY, examine_entry_data, 
                                                                  CJLIB_DICT_NODE_KEY(examine_entry))) return NULL;

                    curr_incomp.set_comma = set_comma_tmp;
                    convert_list_to_queue(curr_incomp.i_pending_data_q, curr_incomp.i_data.array);
                    break;
                default:
                    tmp_state = curr_incomp.i_state;

                    if (CJLIB_OBJECT == curr_incomp.i_type) {
                        if (cjlib_queue_is_empty(curr_incomp.i_pending_data_q)) {
                            value_str = simple_key_value_paired_stringtify(CJLIB_DICT_NODE_DATA(examine_entry), false);
                        } else {
                            value_str = simple_key_value_paired_stringtify(CJLIB_DICT_NODE_DATA(examine_entry), true);
                        }

                        tmp_key = wrap_complete_entry(CJLIB_DICT_NODE_KEY(examine_entry),opening_symbol_str, closing_symbol_str, false);
                        curr_incomp.i_state = incomplete_property_str_expand_state(curr_incomp.i_state, value_str, 
                                                                                   tmp_key);

                        free(tmp_key);
                        tmp_key = NULL;
                    } else {
                        if (cjlib_queue_is_empty(curr_incomp.i_pending_data_q)) {
                            value_str = simple_key_value_paired_stringtify(examine_entry_data, false);
                        } else {
                            value_str = simple_key_value_paired_stringtify(examine_entry_data, true);
                        }

                        curr_incomp.i_state = incomplete_property_str_expand_state(curr_incomp.i_state, value_str, NULL);
                    }
                    free(tmp_state);
                    free(value_str);
                    tmp_state = NULL;
                    value_str = NULL;
                    break;
            }

            set_comma_tmp = true;
        }

        opening_symbol = (CJLIB_OBJECT == curr_incomp.i_type)? opening_symbol_obj : opening_symbol_arr;
        closing_symbol = (CJLIB_OBJECT == curr_incomp.i_type)? closing_symbol_obj : closing_symbol_arr;

        if (CJLIB_OBJECT == curr_incomp.i_type || CJLIB_ARRAY == curr_incomp.i_type) {
            tmp_state = curr_incomp.i_state;

            if (cjlib_stack_is_empty(&incomplete_st) || !curr_incomp.set_comma) {
                curr_incomp.i_state = wrap_complete_entry(curr_incomp.i_state, opening_symbol, closing_symbol, false);
            } else {
                curr_incomp.i_state = wrap_complete_entry(curr_incomp.i_state, opening_symbol, closing_symbol, true);
            }

            free(tmp_state);
            if (NULL == curr_incomp.i_state) return NULL;
        }

        value_str = curr_incomp.i_state;
       
        tmp_key   = curr_incomp.i_key;
        // Free the pending queue (there are no more incomplete data for the JSON object/array that were examined).
        free(curr_incomp.i_pending_data_q);
        curr_incomp.i_pending_data_q = NULL;
    } 

    free(curr_incomp.i_key);

    (void) printf("%s\n", curr_incomp.i_state);
    // Return the now completed JSON.
    return curr_incomp.i_state;
}

int cjlib_json_dump(const struct cjlib_json *restrict src)
{
    if (NULL == freopen(src->c_path, "w+", src->c_fp)) return -1;

    const char *json_content = cjlib_json_object_stringtify(src->c_dict);
    if (NULL == json_content) return -1;

    (void) fwrite((void *) json_content, strlen(json_content), 1, src->c_fp);

    free((void *) json_content);
    return 0;
}
