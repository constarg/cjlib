// @file: cjlib.c

#include <stdio.h>

#include "cjlib.h"
#include "cjlib_dictionary.h"

int cjlib_json_object_set(cjlib_json_object *restrict src, const char *restrict key, 
                          struct cjlib_json_data *restrict value, enum cjlib_json_datatypes datatype)
{
    return 0;
}

int cjlib_json_object_get(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src,
                          const char *restrict key)
{
    return 0;
}

int cjlib_json_object_remove(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src, 
                             const char *key)
{
    return 0;
}

int cjlib_json_open(struct cjlib_json *restrict dst, const char *restrict json_path)
{
    // Initialize the dictionary or the json
    // Recieve a file descriptor, for the file in the json_path.
    return 0;
}

int cjlib_json_close(const struct cjlib_json *restrict src)
{
    // Destroy the dicionary
    // Close the file with the file descriptor
    return 0;
}

int cjlib_json_read(struct cjlib_json *restrict dst)
{
    // 1. Read the contents of the file, spcecified in the file descriptor
    // 2. Parse the json file and build the dictionary in memory.
    return 0;
}

char *cjlib_json_object_stringtify(const cjlib_json_object *restrict src)
{

}

int cjlib_json_dump(const struct cjlib_json *restrict src)
{
    // 1. Translate the dicionary from the memory to json format.
    // 2. Write the resulted string in the json file.
    return 0;
}
