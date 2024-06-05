// @file: cjlib.c

#include <stdio.h>
#include <errno.h>

#include "cjlib.h"
#include "cjlib_dictionary.h"


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

    dst->c_fp       = fp;
    dst->c_filepath = json_path;
    cjlib_dict_init(dst->c_dict);
    return 0;
}

void cjlib_json_close(struct cjlib_json *restrict src)
{
    cjlib_dict_destroy(src->c_dict);
    fclose(src->c_fp);
}

static int preprocessing_json(struct cjlib_json *dst, const struct cjlib_json *src)
{
#if defined(__unix__)
    char *tmp_directory = "/tmp/";
#else
    char *tmp_directory = "C:\\Users\\AppData\\Local\\Temp\\";
#endif

    char *tmp_fileprefix = "cjlib_";
    int file_id = 0;

    char *tmp_filepath = (char *) malloc(strlen(tmp_directory)   + 
                                         strlen(tmp_fileprefix)  + 
                                         strlen(src->c_filepath) + 1);

    sprintf(tmp_filepath, "%s%s%s", tmp_directory, tmp_fileprefix, src->c_filepath);

    FILE *fp = fopen(tmp_filepath, "r");
    while (NULL != (fp = fopen(tmp_filepath, "r"))) {

    }


    free(tmp_filepath);
}

int cjlib_json_read(struct cjlib_json *restrict dst)
{
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
