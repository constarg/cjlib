// @file: cjlib.c

#include "cjlib.h"

int cjlib_json_set_datatype(struct cjlib_json_obj restrict *src, const char restrict *name, 
                            union cjlib_json_datatype restrict *value, enum cjlib_json_datatypes datatype)
{

}

int cjlib_json_get_datatype(union cjlib_json_datatype restrict *dst, const char restrict *name,
                            const struct cjlib_json_obj restrict *json_obj, enum cjlib_json_datatypes datatype)
{
    
}

int cjlib_json_open(struct cjlib_json_obj restrict *dst, const char restrict *json_path)
{

}

int cjlib_json_close(const struct cjlib_json_obj restrict *src)
{

}

int cjlib_json_read(struct cjlib_json_obj restrict *dst)
{

}

int cjlib_json_dump(const struct cjlib_json_obj restrict *src)
{
    
}
