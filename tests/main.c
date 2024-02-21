#include "cjlib.h"



int main(void)
{
    cjlib_json_obj json;
    cjlib_json_datatype data;

    cjlib_json_obj_init(&json);
    cjlib_json_datatype_init(&data);

    cjlib_json_open(&json, "./json_test.json");
    cjlib_json_read(&json);

    cjlib_json_get_datatype(&data, "user_id", &json, CJLIB_NUMBER);

    printf("%d\n", data.c_num);
    cjlib_json_close(&json);
}
