// @file: cjlib.h
#ifndef CJLIB_H
#define CJLIB_H

#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "cjlib_dictionary.h"
#include "cjlib_error.h"

typedef long cjlib_json_num;
typedef bool cjlib_json_bool;

typedef FILE *cjlib_json_fd;
typedef cjlib_dict_t cjlib_json_object;

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

// Some useful macros.
// Retreive the number which is inside the data.
#define CJLIB_GET_NUMBER(CJLIB_DATA) (CJLIB_DATA.c_value.c_num)
// Retreive the string which is inside the data.
#define CJLIB_GET_STRING(CJLIB_DATA) (CJLIB_DATA.c_value.c_str)
// Retreive the boolean which is inside the data.
#define CJLIB_GET_BOOL(CJLIB_DATA)   (CJLIB_DATA.c_value.c_boolean)
// Retreive the object which is inside the data.
#define CJLIB_GET_OBJECT(CJLIB_DATA) (CJLIB_DATA.c_value.c_obj)
// Retreive the Array which is inside the data.
#define CJLIB_GET_ARRAY(CJLIB_DATA)  (CJLIB_DATA.c_value.c_arr)

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
    cjlib_json_fd c_fp;         // The file pointer of the json file.
    cjlib_json_object *c_dict;  // The dictionary that contains the values of the json.
};

/**
 * Json data type. Important for the user who uses this library.
*/
union cjlib_json_data_disting
{
    char *c_str;                           // string
    cjlib_json_num c_num;                  // Number
    cjlib_json_bool c_boolean;             // Boolean.
    cjlib_json_object c_obj;               // json object.
    union cjlib_json_data_disting *c_arr;  // array.
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
    (void)memset(src, 0x0, sizeof(struct cjlib_json));
    src->c_dict = cjlib_make_dict();
    if (NULL == src->c_dict) return -1;
    cjlib_dict_init(src->c_dict);
    return 0;
}

static inline void cjlib_json_destroy(struct cjlib_json *restrict src)
{
    cjlib_dict_destroy(src->c_dict);
    (void)memset(src, 0x0, sizeof(struct cjlib_json));
}

static inline cjlib_json_object *cjlib_json_make_object(void)
{
    cjlib_json_object *obj = cjlib_make_dict();
    cjlib_dict_init(obj);
    return obj;
}

static inline void cjlib_json_data_init(struct cjlib_json_data *restrict src)
{
     (void)memset(src, 0x0, sizeof(struct cjlib_json_data));
}

static inline void cjlib_json_data_destroy(struct cjlib_json_data *restrict src)
{
    switch (src->c_datatype)
    {
        case CJLIB_STRING:
            free(src->c_value.c_arr);
            break;
        case CJLIB_OBJECT:
            cjlib_dict_destroy(&src->c_value.c_obj);
            break;
        case CJLIB_ARRAY:
            free(src->c_value.c_arr);
        default:
            break;
    }
}

// static inline struct cjlib_json_data *cjlib_json_make_array(size_t size) 
// {
//     return (struct cjlib_json_data *) malloc(sizeof(struct cjlib_json_data) * size);
// }

// static inline void cjlib_json_free_array(struct cjlib_json_data *src)
// {
//     free(src);
// }

/**
 * This function acociates a key (string) to a value and set this combination key - value
 * to a json object.
 * 
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value to acociate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_set(cjlib_json_object *src, const char *restrict key, 
                                 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype);

/**
 * This function get the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value acociated with the key.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_get(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src,
                                 const char *restrict key);

/**
 * This function removes the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates a value with it. 
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_remove(struct cjlib_json_data *restrict dst, cjlib_json_object *src, 
                                    const char *key);

/**
 * This function acociates a key (string) to a value and set this combination key - value
 * to a json object.
 * 
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value to acociate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
static CJLIB_ALWAYS_INLINE int cjlib_json_set(struct cjlib_json *restrict src, const char *restrict key, 
                                 struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype)
{
    return cjlib_json_object_set(src->c_dict, key, value, datatype);
}

/**
 * This function get the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value acociated with the key.
 * @return 0 on success, otherwise -1.
*/

static CJLIB_ALWAYS_INLINE int cjlib_json_get(struct cjlib_json_data *restrict dst, const struct cjlib_json *restrict src, 
                                 const char *restrict key)
{
    return cjlib_json_object_get(dst, src->c_dict, key);
}

/**
 * This function removes the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates a value with it. 
 * @return 0 on success, otherwise -1.
*/
static CJLIB_ALWAYS_INLINE int cjlib_json_remove(struct cjlib_json_data *restrict dst, const struct cjlib_json *restrict src, 
                             const char *restrict key)
{
    return cjlib_json_object_remove(dst, src->c_dict, key);
}

/**
 * This function open's a json file.
 * @param dst The json object acociated with the json file.
 * @param json_path The path to the json file of interest.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_open(struct cjlib_json *restrict dst, const char *restrict json_path, 
                           const char *restrict modes);

/**
 * This function close a json file.
 * @param src The json object acociated with the open file.
*/
extern void cjlib_json_close(struct cjlib_json *restrict src);

/**
 * This function read's the contents of a json file.
 * @param dst Where to put all the informations about the json.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_read(struct cjlib_json *restrict dst);

/**
 * This funciton make a json file to string.
 * @param src The json object
 * @return on success, a pointer at the start of a string that represent the string version
 * of the given json file. Otherwise, null.
*/
extern char *cjlib_json_object_stringtify(const cjlib_json_object *restrict src);

static inline char *cjlib_json_stringtify(struct cjlib_json *restrict src)
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
