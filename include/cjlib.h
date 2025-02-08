/* File: cjlib.h
 *
 ************************************************************************
 * Copyright (C) 2024 Constantinos Argyriou
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


#ifndef CJLIB_H
#define CJLIB_H

#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "cjlib_dictionary.h"
#include "cjlib_list.h"
#include "cjlib_error.h"

typedef double cjlib_json_num;
typedef bool cjlib_json_bool;
typedef char *cjlib_json_path;

typedef FILE *cjlib_json_fd;
typedef cjlib_dict_t cjlib_json_object;
typedef struct cjlib_list cjlib_json_array;

#if defined(__GNUC__) || defined(__clang__)

#if defined(CJLIB_ALWAYS_INLINE)
  #undef CJLIB_ALWAYS_INLINE
#endif

#define CJLIB_ALWAYS_INLINE inline __attribute__((__always_inline__))

#else
  #define CJLIB_ALWAYS_INLINE inline
#endif

#if defined(CJLIB_BRANCH_LIKELY)
 #undef CJLIB_BRANCH_LIKELY
#endif

#if defined(CJLIB_BRANCH_UNLIKELY)
 #undef CJLIB_BRANCH_LIKELY
#endif

#if defined(__linux__)

#define CJLIB_BRANCH_LIKELY(x) __builtin_expect(!!(x), 1)
#define CJLIB_BRANCH_UNLIKELY(x) __builtin_expect(!!(x), 0)

#else
 #define CJLIB_BRANCH_LIKELY(x) !!(x)
 #define CJLIB_BRANCH_UNLIKELY(x) !!(x)
#endif

#if defined(CJLIB_ARR_FOR_EACH)
 #undef CJLIB_ARR_FOR_EACH
#endif

#if defined(CJLIB_GET_NUMBER)
 #undef CJLIB_GET_NUMBER
#endif

#if defined(CJLIB_GET_STRING)
 #undef CJLIB_GET_STRING
#endif

#if defined(CJLIB_GET_BOOL)
 #undef CJLIB_GET_BOOL
#endif

#if defined(CJLIB_GET_OBJ)
 #undef CJLIB_GET_OBJ
#endif

#if defined(CJLIB_GET_ARR)
 #undef CJLIB_GET_ARR
#endif

// For each for array access.
#define CJLIB_ARR_FOR_EACH(ITEM, ARR_PTR, TYPE) (CJLIB_LIST_FOR_EACH(ITEM, ARR_PTR, TYPE))

// Some useful macros.
// Retrieve the number which is inside the data.
#define CJLIB_GET_NUMBER(CJLIB_DATA) (CJLIB_DATA.c_value.c_num)
// Retrieve the string which is inside the data.
#define CJLIB_GET_STRING(CJLIB_DATA) (CJLIB_DATA.c_value.c_str)
// Retrieve the boolean which is inside the data.
#define CJLIB_GET_BOOL(CJLIB_DATA)   (CJLIB_DATA.c_value.c_boolean)
// Retrieve the object which is inside the data.
#define CJLIB_GET_OBJECT(CJLIB_DATA) (CJLIB_DATA.c_value.c_obj)
// Retrieve the Array which is inside the data.
#define CJLIB_GET_ARRAY(CJLIB_DATA)  (CJLIB_DATA.c_value.c_arr)

#define CJLIB_ARRAY_INIT_SIZE 200

/**
 * The available datatypes, that json support.
*/
enum cjlib_json_datatypes
{
    CJLIB_STRING,
    CJLIB_NUMBER,
    CJLIB_ARRAY,
    CJLIB_BOOLEAN,
    CJLIB_OBJECT,
    CJLIB_NULL
};

/**
 * The json object. !Library user must ignore this!.
*/
struct cjlib_json
{
    cjlib_json_fd c_fp;        // The file pointer of the json file.
    cjlib_json_object *c_dict; // The dictionary that contains the values of the json.
    char *c_path;              // The path to the respective file.
};

/**
 * Json data type. Important for the user who uses this library.
*/
union cjlib_json_data_disting
{
    char *c_str;               // string
    cjlib_json_num c_num;      // Number
    cjlib_json_bool c_boolean; // Boolean.
    cjlib_json_object *c_obj;  // json object.
    void *c_null;              // null
    cjlib_json_array *c_arr;   // array.
};

/**
 * This structure represents the data retrieved or stored from a JSON file.
 */
struct cjlib_json_data
{
    union cjlib_json_data_disting c_value; 
    enum cjlib_json_datatypes c_datatype;
};

static inline int cjlib_json_init(struct cjlib_json *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_json));
    src->c_dict = cjlib_make_dict();
    if (NULL == src->c_dict) return -1;
    cjlib_dict_init(src->c_dict);
    return 0;
}

static inline void cjlib_json_destroy(struct cjlib_json *restrict src)
{
    cjlib_dict_destroy(src->c_dict);
    src->c_dict = NULL;
    free(src->c_path);
    src->c_path = NULL;
}

static inline cjlib_json_object *cjlib_json_make_object(void)
{
    cjlib_json_object *obj = cjlib_make_dict();
    cjlib_dict_init(obj);
    return obj;
}

static inline void cjlib_json_data_init(struct cjlib_json_data *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_json_data));
}

static void cjlib_json_free_array(cjlib_json_array *src);

static inline void cjlib_json_data_destroy(struct cjlib_json_data *restrict src)
{
    if (NULL == src) return;

    switch (src->c_datatype) {
        case CJLIB_STRING:
            free(src->c_value.c_str);
            break;
        case CJLIB_OBJECT:
            cjlib_dict_destroy(src->c_value.c_obj);
            break;
        case CJLIB_ARRAY:
            cjlib_json_free_array(src->c_value.c_arr);
            break;
        default:
            break;
    }
}

static inline void cjlib_array_free_data(void *restrict src)
{
    struct cjlib_json_data *data = (struct cjlib_json_data *) src;
    cjlib_json_data_destroy(data);
}

static inline void cjlib_json_free_array(cjlib_json_array *src)
{
    cjlib_list_destroy(src, &cjlib_array_free_data);
}

static inline cjlib_json_array *cjlib_json_make_array(void)
{
    cjlib_json_array *arr = make_list();
    cjlib_list_init(arr);

    return arr;
}

static inline int cjlib_json_array_append(cjlib_json_array *restrict src, const struct cjlib_json_data *restrict value)
{
    return cjlib_list_append((const void *) value, sizeof(struct cjlib_json_data), src);
}

static inline int cjlib_json_array_get(struct cjlib_json_data *restrict dst, int index, cjlib_json_array *restrict arr)
{
    return cjlib_list_get(dst, sizeof(struct cjlib_json_data), index, arr);
}

/**
 * This function acociates a key (string) to a value and set this combination key - value
 * to a json object.
 * 
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value to associate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_set
(cjlib_json_object **src, const char *restrict key,
 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype);

/**
 * This function get the data associated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value associated with the key.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_get
(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src,
 const char *restrict key);

/**
 * This function removes the data associated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates a value with it. 
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_remove
(struct cjlib_json_data *restrict dst, cjlib_json_object **src,
 const char *key);

/**
 * This function acociates a key (string) to a value and set this combination key - value
 * to a json object.
 * 
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value to associate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
static CJLIB_ALWAYS_INLINE int cjlib_json_set
(struct cjlib_json *restrict src, const char *restrict key,
 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype)
{
    return cjlib_json_object_set(&src->c_dict, key, value, datatype);
}

/**
 * This function get the data associated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value associated with the key.
 * @return 0 on success, otherwise -1.
*/

static CJLIB_ALWAYS_INLINE int cjlib_json_get
(struct cjlib_json_data *restrict dst, const struct cjlib_json *restrict src,
 const char *restrict key)
{
    return cjlib_json_object_get(dst, src->c_dict, key);
}

/**
 * This function removes the data associated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates a value with it. 
 * @return 0 on success, otherwise -1.
*/
static CJLIB_ALWAYS_INLINE int cjlib_json_remove
(struct cjlib_json_data *restrict dst, struct cjlib_json *restrict src,
 const char *restrict key)
{
    return cjlib_json_object_remove(dst, &src->c_dict, key);
}

/**
 * This function open's a json file.
 * @param dst The json object associated with the json file.
 * @param json_path The path to the json file of interest.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_open
(struct cjlib_json *restrict dst, const char *restrict json_path,
 const char *restrict modes);

/**
 * This function close a json file.
 * @param src The json object associated with the open file.
*/
extern void cjlib_json_close(struct cjlib_json *restrict src);

/**
 * This function read's the contents of a json file.
 * @param dst Where to put all the information's about the json.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_read(struct cjlib_json *restrict dst);

/**
 * This function make a json file to string.
 * @param src The json object
 * @return on success, a pointer at the start of a string that represent the string version
 * of the given json file. Otherwise, null.
*/
extern const char *cjlib_json_object_stringtify(const cjlib_json_object *src);

static inline const char *cjlib_json_stringtify(struct cjlib_json *src)
{
    return cjlib_json_object_stringtify(src->c_dict);
}

/**
 * This function write back the contents of the json.
 * @param src The json to write back.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_dump(const struct cjlib_json *restrict src);


#endif
