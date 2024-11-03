/* File: cjlib_error.h
 *
 * Copyright (C) 2024 Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CJLIB_ERROR
#define CJLIB_ERROR

#include <memory.h>
#include <malloc.h>

enum cjlib_json_error_types
{
    NO_ERROR,
    UNDEFINED,
    INVALID_TYPE,
    INVALID_JSON,
    DUPLICATE_NAME,
    INVALID_PROPERTY,
    MISSING_SEPERATOR,
    MEMORY_ERROR,
    INCOMPLETE_CURLY_BRACKETS,
    INCOMPLETE_SQUARE_BRACKETS,
    INCOMPLETE_DOUBLE_QUOTES,
    MISSING_COMMA,
    INVALID_NUMBER
};

struct cjlib_json_error
{
    char *c_property_name;                    // The name of the property in which the error occurred.
    char *c_property_value;                   // The value of the property in which the error occurred.
    enum cjlib_json_error_types c_error_code; // A error code indicating the error.
};

extern int cjlib_json_error_init(void);

extern void cjlib_json_error_destroy(void);

extern void cjlib_setup_error
(const char *property_name, const char *property_value,
 enum cjlib_json_error_types error_code);

extern void cjlib_json_get_error(struct cjlib_json_error *restrict dst);

extern enum cjlib_json_error_types cjlib_error_indicator_correction(int func_err_code);

#endif
