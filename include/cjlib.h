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

typedef double cjlib_json_num; /* Represents a JSON number. */
typedef bool cjlib_json_bool;  /* Represents a JSON boolean entry. */
typedef char *cjlib_json_path; /* A string that represents the location of the JSON file. */

typedef FILE *cjlib_json_fd;                /* The file descriptor of the JSON file. */
typedef cjlib_dict_t cjlib_json_object;     /* Represents a JSON object. */
typedef struct cjlib_list cjlib_json_array; /* Represents a JSON array. */

#if defined(__GNUC__) || defined(__clang__)

#if defined(CJLIB_ALWAYS_INLINE)
  #undef CJLIB_ALWAYS_INLINE
#endif

// CJLIB_ALWAYS_INLINE is used to optimize inline functions.
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

/**
 * CJLIB_BRANCH_LIKELY is used to optimize a branch operation
 * in which a condition is commonly true.
 *
 * @param x The variable that determines if the code in the branch is going to be executed.
 */
#define CJLIB_BRANCH_LIKELY(x) __builtin_expect(!!(x), 1)

/**
 * CJLIB_BRANCH_UNLIKELY is used to optimize a branch operation
 * in which a condition is not commonly true.
 *
 * @param x The variable that determines if the code in the branch is going to be executed.
 */
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

/**
 * CJILB_ARR_FOR_EACH iterates on each element of the array ARR_PTR and store
 * the contents of the current element in the variable ITEM.
 *
 * @param ITEM Represents the currently examined element of the array.
 * @param ARR_PTR Represents a pointer to the array of interest.
 * @param TYPE Represents the data-type of the items present in the array.
 */
#define CJLIB_ARR_FOR_EACH(ITEM, ARR_PTR, TYPE) (CJLIB_LIST_FOR_EACH(ITEM, ARR_PTR, TYPE))

/**
 * CJLIB_GET_NUMBER retrieves a number data-type from the cjlib_data structure.
 * It could be used as a more readable version of its equivalent: CJLIB_DATA.c_value.c_num.
 *
 * @param CJLIB_DATA A variable declared as a type of cjlib_json_data structure.
 */
#define CJLIB_GET_NUMBER(CJLIB_DATA) (CJLIB_DATA.c_value.c_num)

/**
 * CJLIB_GET_STRING retrieves a string data-type from the cjlib_data structure.
 * It could be used as a more readable version of its equivalent: CJLIB_DATA.c_value.c_str.
 *
 * @param CJLIB_DATA A variable declared as a type of cjlib_json_data structure.
 */
#define CJLIB_GET_STRING(CJLIB_DATA) (CJLIB_DATA.c_value.c_str)

/**
 * CJLIB_GET_BOOL retrieves a boolean data-type from the cjlib_data structure.
 * It could be used as a more readable version of its equivalent: CJLIB_DATA.c_value.c_boolean.
 *
 * @param CJLIB_DATA A variable declared as a type of cjlib_json_data structure.
 */
#define CJLIB_GET_BOOL(CJLIB_DATA)   (CJLIB_DATA.c_value.c_boolean)

/**
 * CJLIB_GET_OBJECT retrieves an object data-type from the cjlib_data structure.
 * It could be used as a more readable version of its equivalent: CJLIB_DATA.c_value.c_obj.
 *
 * @param CJLIB_DATA A variable declared as a type of cjlib_json_data structure.
 */
#define CJLIB_GET_OBJECT(CJLIB_DATA) (CJLIB_DATA.c_value.c_obj)

/**
 * CJLIB_GET_ARRAY retrieves an array data-type from the cjlib_data structure.
 * It could dbe used as a more readable version of its equivalent: CJLIB_DATA.c_value.c_arr
 *
 * @param CJLIB_DATA A variable declared as a type of cjlib_json_data structure.
 */
#define CJLIB_GET_ARRAY(CJLIB_DATA)  (CJLIB_DATA.c_value.c_arr)

/**
 * CJLIB_INIT_SIZE determines an initial number of elements for a new array
 */
#define CJLIB_ARRAY_INIT_SIZE 200

/**
 * cjlib_json_datatypes enumeration declares the list of available
 * data-types in JSON standard.
*/
enum cjlib_json_datatypes
{
    CJLIB_STRING,  /* Represents the STRING data-type. */
    CJLIB_NUMBER,  /* Represents the INTEGER/FLOAT data-type. */
    CJLIB_ARRAY,   /* Represents the ARRAY data-type. */
    CJLIB_BOOLEAN, /* Represents the BOOLEAN data-type. */
    CJLIB_OBJECT,  /* Represents the OBJECT data-type. */
    CJLIB_NULL     /* Represents the NULL data-type. */
};

/**
 *  cjlib_json represents the JSON representation stored in memory.
*/
struct cjlib_json
{
    cjlib_json_fd c_fp;        /* Represents the file pointer of the JSON file. */
    cjlib_json_object *c_dict; /* Represents the root-object containing all the entries. */
    char *c_path;              /* Represents the path to the JSON file. */
};

/**
 * cjlib_json_data_disting is used to differentiate between data-types.
*/
union cjlib_json_data_disting
{
    char *c_str;               /* Represents a STRING data-type. */
    cjlib_json_num c_num;      /* Represents a INTEGER/FLOAT data-type. */
    cjlib_json_bool c_boolean; /* Represents a BOOLEAN data-type. */
    cjlib_json_object *c_obj;  /* Represents an OBJECT data-type. */
    void *c_null;              /* Represents a NULL data-type. */
    cjlib_json_array *c_arr;   /* Represents an ARRAY data-type. */
};

/**
 * cjlib_json_data represents an entry of the JSON file.
 */
struct cjlib_json_data
{
    union cjlib_json_data_disting c_value; /* Represents the value of the entry. */
    enum cjlib_json_datatypes c_datatype;  /* Represents the data-type of the value. */
    /*
     * c_value constitute of a type specified in the cjlib_json_data_disting union, thus
     * a second field, c_datatype, is required to know the selected data-type.
     */
};

/**
 * cjlib_json_init Initializes a JSON structure.
 *
 * @param src A pointer to the memory area where the JSON representation in memory is stored.
 * @return An integer indicating whether the operation were succeed. 0 is returned on success,
 * otherwise -1.
 */
static inline int cjlib_json_init(struct cjlib_json *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_json));
    src->c_dict = cjlib_make_dict();
    if (NULL == src->c_dict) return -1;
    cjlib_dict_init(src->c_dict);
    return 0;
}

/**
 * cjlib_json_destroy is used to free the memory allocated by the JSON.
 *
 * @param src A pointer to the memory area where the JSON representation in memory is stored.
 */
static inline void cjlib_json_destroy(struct cjlib_json *restrict src)
{
    cjlib_dict_destroy(src->c_dict);
    src->c_dict = NULL;
    free(src->c_path);
    src->c_path = NULL;
}

/**
 * cjlib_json_make_object is leveraged for creating a new JSON object.
 *
 * @return A pointer to the newly created JSON object.
 */
static inline cjlib_json_object *cjlib_json_make_object(void)
{
    cjlib_json_object *obj = cjlib_make_dict();
    cjlib_dict_init(obj);
    return obj;
}

/**
 * cjlib_json_data_init Initializes the structure responsible for representation
 * a JSON data-type.
 *
 * @param src A pointer to the memory area where the JSON data entry is stored.
 */
static inline void cjlib_json_data_init(struct cjlib_json_data *restrict src)
{
    (void) memset(src, 0x0, sizeof(struct cjlib_json_data));
}

/**
 * cjlib_json_free_array is used to free the memory allocated by an
 * array data-type.
 *
 * @param src A pointer to the memory area where the array is stored.
 */
static void cjlib_json_free_array(cjlib_json_array *src);

/**
 * cjlib_json_data_destroy is used to free the memory allocated by a JSON
 * entry.
 *
 * @param src A pointer to the memory area where the JSON entry is stored.
 */
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

/**
 * cjlib_array_free_data Is used to free the JSON entries
 * stored in an array.
 *
 * @param src A pointer to the memory area where the JSON entry is stored.
 */
static inline void cjlib_array_free_data(void *restrict src)
{
    struct cjlib_json_data *data = (struct cjlib_json_data *) src;
    cjlib_json_data_destroy(data);
}

static inline void cjlib_json_free_array(cjlib_json_array *src)
{
    cjlib_list_destroy(src, &cjlib_array_free_data);
}

/**
 * cjlib_json_make_array create a new array data-type entry
 *
 * @return A pointer to the memory area where the newly created array-type is stored.
 */
static inline cjlib_json_array *cjlib_json_make_array(void)
{
    cjlib_json_array *arr = make_list();
    cjlib_list_init(arr);

    return arr;
}

/**
 * cjlib_json_array_append appends an ew element at the end of an array.
 *
 * @param src A pointer to the memory area where the array to append the new element is stored.
 * @param value The value to store into the array.
 * @return An integer indicating whether the operation were successfully. On success 0 is returned, otherwise
 * -1.
 */
static inline int cjlib_json_array_append(cjlib_json_array *restrict src, const struct cjlib_json_data *restrict value)
{
    return cjlib_list_append((const void *) value, sizeof(struct cjlib_json_data), src);
}

/**
 * cjlib_json_array_get retrieves an element from an array based on the index requested.
 *
 * @param dst A pointer to the memory area where the data retrieved will be stored.
 * @param index An integer representing the index of the element.
 * @param arr A pointer to the memory area where the array is stored.
 * @return An integer indicating whether the element is found or not. On success 0 is returned, otherwise
 * -1.
 */
static inline int cjlib_json_array_get(struct cjlib_json_data *restrict dst, int index, cjlib_json_array *restrict arr)
{
    return cjlib_list_get(dst, sizeof(struct cjlib_json_data), index, arr);
}

/**
 * This function associates a key (string) to a value and set this combination key - value
 * to a json object.
 * 
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that associates the value with it.
 * @param value The value to associate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_set
(cjlib_json_object **src, const char *restrict key,
 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype);

/**
 * This function get the data associated with the key.
 *
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that associates the value with it.
 * @param value The value associated with the key.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_get
(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src,
 const char *restrict key);

/**
 * cjlib_json_object_remove removes the data associated with the key.
 *
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that associates a value with it.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_remove
(struct cjlib_json_data *restrict dst, cjlib_json_object **src,
 const char *key);

/**
 * This function associates a key (string) to a value and set this combination key - value
 * to a json object.
 * 
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that associates the value with it.
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
 *
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that associates the value with it.
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
 *
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that associates a value with it.
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
 *
 * @param dst The json object associated with the json file.
 * @param json_path The path to the json file of interest.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_open
(struct cjlib_json *restrict dst, const char *restrict json_path,
 const char *restrict modes);

/**
 * This function close a json file.
 *
 * @param src The json object associated with the open file.
*/
extern void cjlib_json_close(struct cjlib_json *restrict src);

/**
 * This function read's the contents of a json file.
 *
 * @param dst Where to put all the information's about the json.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_read(struct cjlib_json *restrict dst);

/**
 * This function make a json file to string.
 *
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
