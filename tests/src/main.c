#include <stdio.h>
#include <stdlib.h>

#include "cjlib.h"
#include "cjlib_dictionary.h"
#include "cjlib_queue.h"

// Transform the low level CJLIB_GET_STRING to highier level GET_FIRST_NAME.
#define GET_FIRST_NAME(JSON_DATA) \
    CJLIB_GET_STRING(JSON_DATA)

int main(void)
{
    cjlib_dict *dict = cjlib_make_dict();
    cjlib_dict_init(dict);

    struct cjlib_json_data test_data1;
    struct cjlib_json_data test_data2;
    struct cjlib_json_data test_data3;
    struct cjlib_json_data test_data4;
    struct cjlib_json_data test_data5;
    struct cjlib_json_data test_data6;
    struct cjlib_json_data test_data7;
    struct cjlib_json_data test_data8;
    struct cjlib_json_data test_data9;

    test_data1.c_datatype = CJLIB_NUMBER;
    test_data1.c_value.c_num = 10;

    test_data2.c_datatype = CJLIB_NUMBER;
    test_data2.c_value.c_num = 22;

    test_data3.c_datatype = CJLIB_NUMBER;
    test_data3.c_value.c_num = 59;

    test_data4.c_datatype = CJLIB_NUMBER;
    test_data4.c_value.c_num = 121;

    test_data5.c_datatype = CJLIB_NUMBER;
    test_data5.c_value.c_num = 255;

    test_data6.c_datatype = CJLIB_NUMBER;
    test_data6.c_value.c_num = 1204;

    test_data7.c_datatype = CJLIB_NUMBER;
    test_data7.c_value.c_num = 44;

    test_data8.c_datatype = CJLIB_NUMBER;
    test_data8.c_value.c_num = 595;

    test_data9.c_datatype = CJLIB_NUMBER;
    test_data9.c_value.c_num = 53;

    cjlib_dict_insert(&test_data1, &dict, "10");
    cjlib_dict_insert(&test_data2, &dict, "22");
    cjlib_dict_insert(&test_data3, &dict, "59");
    cjlib_dict_insert(&test_data4, &dict, "79");
    cjlib_dict_insert(&test_data4, &dict, "25");
    cjlib_dict_insert(&test_data5, &dict, "120");
    cjlib_dict_insert(&test_data6, &dict, "124");
    cjlib_dict_insert(&test_data7, &dict, "5");
    cjlib_dict_insert(&test_data8, &dict, "15");
    cjlib_dict_insert(&test_data9, &dict, "434");

    get_height(dict);

    printf("ROOT KEY: %s\n", (dict)->avl_key);
    printf("RIGHT KEY: %s\n", (dict)->avl_right->avl_key);
    printf("LEFT KEY: %s\n", (dict)->avl_left->avl_key);

    struct cjlib_json_data test_data_s1;

    cjlib_dict_search(&test_data_s1, dict, "59");
    printf("Result from search: %d\n", test_data_s1.c_value.c_num);

    cjlib_dict_search(&test_data_s1, dict, "25");
    printf("Result from search: %d\n", test_data_s1.c_value.c_num);

    cjlib_dict_destroy(dict);
    // struct cjlib_json json_file;
    // struct cjlib_json_data dst;

    // // Initiazlie the json file.
    // cjlib_json_init(&json_file);
    // cjlib_json_data_init(&dst);

    // // Open the json file of interest.
    // if (-1 == cjlib_json_open(&json_file, "./input/example.json", "r")) {
    //     (void)printf("Failed to open the json file\n");
    //     exit(0);
    // }

    // if (-1 == cjlib_json_get(&dst, &json_file, "first_name")) {
    //     (void)printf("Failed to retrieve key");
    //     cjlib_json_close(&json_file);
    //     exit(0);
    // }

    // (void)printf("NAME: %s\n", GET_FIRST_NAME(dst));

    // // Close the json file.
    // cjlib_json_close(&json_file);
}
