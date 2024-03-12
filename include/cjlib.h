// @file: cjlib.h
#ifndef CJLIB_H
#define CJLIB_H

#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

#include "cjlib_dictionary.h"

typedef int cjlib_json_num;
typedef bool cjlib_json_bool;

typedef int cjlib_json_fd;
typedef cjlib_dict cjlib_json_object;

// Some useful macros.
#define CJLIB_GETNUMBER(DATA) DATA.c_num
#define CJLIB_GETSTRING(DATA) DATA.c_str
#define CJLIB_GETBOOL(DATA)   DATA.c_boolean
#define CJLIB_GETOBJ(DATA)    DATA.c_obj
#define CJLIB_GETARR(DATA)    DATA.c_arr

/**
 * The available datatypes, that json support.
*/
enum cjlib_json_datatypes
{
    CJLIB_STRING,
    CJLIB_NUMBER,
    CJLIB_ARRAY,
    CJLIB_OBJECT,
    CJLIB_NULL
};

/**
 * The json object. !Library user must ignore this!.
*/
struct cjlib_json
{
    cjlib_json_fd c_fd;        // The file descriptor of the json file.
    cjlib_json_object c_dict;  // The dictionary that contains the values of the json.
};

/**
 * Json data type. Important for the user who uses this library.
*/
union cjlib_json_data
{
    char *c_str;                      // string
    cjlib_json_num c_num;             // Number
    cjlib_json_bool c_boolean;        // Boolean.
    cjlib_json_object c_obj;          // json object.
    union cjlib_json_data *c_arr;     // array.
};

/**
 * This structure is a supportive structure which is
 * used in order to know what datatype is chosed
 * from the available options on the union, cjlib_json_datatype.
*/
struct cjlib_json_data_ext
{
    union cjlib_json_data c_datatype_value;
    enum cjlib_json_datatypes c_datatype;
};

static inline void cjlib_json_init(struct cjlib_json *restrict src)
{
    (void)memset(src, 0x0, sizeof(struct cjlib_json));
}

static inline void cjlib_json_object_init(cjlib_json_object *restrict src)
{
    cjlib_dict_init(src);
}

/**
 * @param src The datatype to initialize. 
*/
static inline void cjlib_json_data_init(union cjlib_json_data *restrict src)
{
    (void)memset(src, 0x0, sizeof(union cjlib_json_data));
}

/**
 * @param size The number of elements of the array.
 * @return A pointer to the newly created array on success. On error NULL.
*/
static inline union cjlib_json_data *cjlib_json_make_array(size_t size) 
{
    return (union cjlib_json_data *) malloc(sizeof(union cjlib_json_data) * size);
}

/**
 * @param src The array to be freed.
*/
static inline void cjlib_json_free_array(union cjlib_json_data *src)
{
    free(src);
}

/**
 * This function acociates a key (string) to a value and set this combination key - value
 * to a json object.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value to acociate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_set(cjlib_json_object *restrict src, const char *restrict key, 
                                 struct cjlib_json_data_ext *restrict value, enum cjlib_json_datatypes datatype);

/**
 * This function get the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value acociated with the key.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_get(struct cjlib_json_data_ext *restrict dst, const cjlib_json_object *restrict src,
                                 const char *restrict key);

/**
 * This function removes the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates a value with it. 
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_object_remove(struct cjlib_json_data_ext *restrict dst, const cjlib_json_object *restrict src, 
                                    const char *key);

/**
 * This function acociates a key (string) to a value and set this combination key - value
 * to a json object.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value to acociate with the key.
 * @param datatype The json datatype of the value.
 * @return 0 on success, otherwise -1.
*/
static inline int cjlib_json_set(struct cjlib_json *restrict src, const char *restrict key, 
                                 struct cjlib_json_data_ext *restrict value, enum cjlib_json_datatypes datatype)
{
    return cjlib_json_object_set(&src->c_dict, key, value, datatype);
}

/**
 * This function get the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored.
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates the value with it.
 * @param value The value acociated with the key.
 * @return 0 on success, otherwise -1.
*/
static inline int cjlib_json_get(struct cjlib_json_data_ext *restrict dst, const struct cjlib_json *restrict src, 
                                 const char *restrict key)
{
    return cjlib_json_object_get(dst, &src->c_dict, key);
}
/**
 * This function removes the data acociated with the key.
 * @param dst A pointer that points to the location where the retrieved data should be stored. (can be NULL).
 * @param src A pointer to the json object in which we should put the value.
 * @param key A string that acociates a value with it. 
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_remove(struct cjlib_json_data_ext *restrict dst, const struct cjlib_json *restrict src, 
                                    const char *key)
{
    return cjlib_json_object_remove(dst, &src->c_dict, key);
}

/**
 * This function open's a json file.
 * @param dst The json object acociated with the json file.
 * @param json_path The path to the json file of interest.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_open(struct cjlib_json *restrict dst, const char *restrict json_path);

/**
 * This function close a json file.
 * @param src The json object acociated with the open file.
*/
extern int cjlib_json_close(const struct cjlib_json *restrict src);

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
extern char *cjlib_json_stringtify(const cjlib_json_object *restrict src);

/**
 * This function write back the contents of the json.
 * @param src The json to write back.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_dump(const struct cjlib_json *restrict src);


#endif
