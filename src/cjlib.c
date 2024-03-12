// @file: cjlib.c

#include "cjlib.h"

int cjlib_json_set_datatype(cjlib_json_object *restrict src, const char *restrict name, 
                            union cjlib_json_datatype *restrict value, enum cjlib_json_datatypes datatype)
{

}

int cjlib_json_get_datatype(union cjlib_json_datatype *restrict dst, const char *restrict name,
                            const cjlib_json_object *restrict json_obj, enum cjlib_json_datatypes datatype)
{
    
}

int cjlib_json_open(cjlib_json_object *restrict dst, const char *restrict json_path)
{

}

int cjlib_json_close(const cjlib_json_object *restrict src)
{

}

int cjlib_json_read(struct cjlib_json *restrict dst)
{

}

int cjlib_json_dump(const struct cjlib_json *restrict src)
{
    
}
