/* File: cjlib_stack.h
 *
 ************************************************************************
 * Copyright (C) 2024 Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "cjlib.h"

#include "cjlib_list.h"


// Transform the low level CJLIB_GET_STRING to higher level GET_FIRST_NAME.
#define GET_FIRST_NAME(JSON_DATA) \
    CJLIB_GET_STRING(JSON_DATA)

#define GET_AGE(JSON_DATA) \
    CJLIB_GET_NUMBER(JSON_DATA)

int main(void)
{
    struct cjlib_json json_file;
    struct cjlib_json_data dst;

    // // Initialize the json file.
    cjlib_json_init(&json_file);
    cjlib_json_data_init(&dst);

    // // Open the json file of interest.
    if (-1 == cjlib_json_open(&json_file, "../input/example.json", "r")) {
        (void) printf("Failed to open the json file\n");
        exit(-1);
    }

    cjlib_json_read(&json_file);

    /*if (-1 == cjlib_json_get(&dst, &json_file, "programming_languages")) {
        (void) printf("Error\n");
        exit(-1);
    }

    struct cjlib_json_data test;
    CJLIB_LIST_FOR_EACH(test, dst.c_value.c_arr, struct cjlib_json_data) {
        (void) printf("%s\n", test.c_value.c_str);
    }

    if (-1 == cjlib_json_get(&dst, &json_file, "industry")) {
        (void) printf("Error\n");
        exit(-1);
    }

    (void) printf("%s\n", dst.c_value.c_str);*/


    if (-1 == cjlib_json_get(&dst, &json_file, "configurations")) {
        (void) printf("Error\n");
        exit(-1);
    }

    /*if (-1 == cjlib_json_array_get(&dst, 0, dst.c_value.c_arr)) {
        (void) printf("Error\n");
        exit(-1);
    }*/

    // // Close the json file.
    cjlib_json_close(&json_file);
}