#include <stdio.h>
#include <stdlib.h>

#include "cjlib.h"


// Transform the low level CJLIB_GET_STRING to highier level GET_FIRST_NAME.
#define GET_FIRST_NAME(JSON_DATA) \
    CJLIB_GET_STRING(JSON_DATA)

int main(void)
{
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
