#ifndef CJLIB_H
#define CJLIB_H

#include <stdbool.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

typedef int cjlib_json_num;
typedef bool cjlib_json_bool;

typedef int cjlib_json_fd;

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
struct cjlib_json_obj
{
    cjlib_json_fd fd; // The file descriptor of the json file.

};

/**
 * Json data type. Important for the user who uses this library.
*/
union cjlib_json_datatype
{
    char *str;                          // string
    cjlib_json_num num;                 // Number
    cjlib_json_bool boolean;            // Boolean.
    struct cjlib_json obj;              // json object.
    struct cjlib_json_datatype *arr;    // array.
};


/**
 * @param src The object to initialize.
*/
static void cjlib_json_obj_init(struct cjlib_json_obj restrict *src)
{
    (void)memset(src, 0x0, sizeof(struct cjlib_json_obj));
}

/**
 * @param src The datatype to initialize. 
*/
static inline void cjlib_json_array_element_init(union cjlib_json_datatype restrict *src)
{
    (void)memset(src, 0x0, sizeof(union cjlib_json_datatype));
}

/**
 * @param size The number of elements of the array.
 * @return A pointer to the newly created array on success. On error NULL.
*/
static inline union cjlib_json_datatype *cjlib_json_make_array(size_t size) 
{
    return (union cjlib_json_datatype *) malloc(sizeof(union cjlib_json_datatype));
}

/**
 * @param src The array to be freed.
*/
static inline void cjlib_json_free_array(union cjlib_json_datatype *src)
{
    free(src);
}

/**
 * @param src   The json object.
 * @param name  The name to acociate the number.
 * @param value The value to acociate with the @param name
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_set_datatype(struct cjlib_json_obj restrict *src, const char restrict *name, 
                                   union cjlib_json_datatype restrict *value, enum cjlib_json_datatypes datatype);

/**
 * @param dst Where to store the requested data.
 * @param name The that is acociated with the data in question.
 * @param json_obj The object which contains the data.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_get_datatype(union cjlib_json_datatype restrict *dst, const char restrict *name,
                                   const struct cjlib_json_obj restrict *json_obj, enum cjlib_json_datatypes datatype);

/**
 * @param dst The json object acociated with the json file.
 * @param json_path The path to the json file of interest.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_open(struct cjlib_json_obj restrict *dst, const char restrict *json_path);

/**
 * @param src The json object acociated with the open file.
*/
extern int cjlib_json_close(const struct cjlib_json_obj restrict *src);

/**
 * @param dst Where to put all the informations about the json.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_read(struct cjlib_json_obj restrict *dst);

/**
 * @param src The json to write back.
 * @return 0 on success, otherwise -1.
*/
extern int cjlib_json_dump(const struct cjlib_json_obj restrict *src);


#endif