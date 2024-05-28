// @file: cjlib.c

#include <stdio.h>

#include "cjlib.h"
#include "cjlib_dictionary.h"

struct json_key_value_pair
{
    unsigned char *j_name;
    unsigned char *j_value;
};

struct cjlib_incomp_object
{
    char *incmp_identifier;   // The identifier related to the object.
    cjlib_json_object *incmp_object; // The object that stopped expanding.
};


int cjlib_json_object_set(cjlib_json_object *restrict src, const char *restrict key, 
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

    if (-1 == cjlib_dict_search(&tmp, src, key)) return -1;

    (void)memcpy(dst, tmp, sizeof(struct cjlib_json_data));
    return 0;
}

int cjlib_json_object_remove(struct cjlib_json_data *restrict dst, const cjlib_json_object *restrict src, 
                             const char *key)
{
    // dst == NULL, then you can skip the return value.
    if (NULL == dst) goto perform_deletion;

    if (-1 == cjlib_json_get(dst, src, key)) return -1;

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

void cjlib_json_close(const struct cjlib_json *restrict src)
{
    cjlib_dict_destroy((cjlib_dict_t *) &src->c_dict);
    fclose(src->c_fp);
}

// static void json_object_tracking(const struct json_key_value_pair *src)
// {
//     // todo - keep track of which object we are expading at the time.
// }

// static inline int decode_to_key_value_pair(struct json_key_value_pair *dst, const char *src)
// {
//     // todo - get a input such as "name":value and convert it into a json_key_value_pair.
// }

int cjlib_json_read(struct cjlib_json *restrict dst)
{
    // 1. Read the contents of the file, spcecified in the file descriptor
    // 2. Parse the json file and build the dictionary in memory.
    unsigned char next_byte;

    while (EOF != (next_byte = (unsigned char) fgetc(dst->c_fp))) {
        
    }


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
