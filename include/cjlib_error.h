/* File: cjlib_error.h
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

#ifndef CJLIB_ERROR
#define CJLIB_ERROR

#include <memory.h>
#include <malloc.h>

/**
 * cjlib_json_error_types enumeration list all the possible errors that can be
 * produced during the parsing of in a transaction between a JSON file.
 */
enum cjlib_json_error_types
{
    NO_ERROR,                     /* No error is produced */
    UNDEFINED,                    /* The error is undefined. */
    INVALID_TYPE,                 /* The type read is not one of the JSON-standard types. */
    INVALID_JSON,                 /* THe JSON read is not valid. */
    DUPLICATE_NAME,               /* The JSON file read contains two entries in the same object with the same name. */
    INVALID_PROPERTY,             /* The is somthing missing from the property, like quotes, comma, or colon. */
    MISSING_SEPERATOR,            /* An entry have no seperator, colon. */
    MEMORY_ERROR,                 /* Some memory error is produced during the parsing. */
    INCOMPLETE_CURLY_BRACKETS,    /* An object in the JSON file have no matching curly brackets. */
    INCOMPLETE_SQUARE_BRACKETS,   /* An array in the JSON file have no matching square brackets */
    INCOMPLETE_DOUBLE_QUOTES,     /* An entry in the JSON file have no matching double quotes. */
    MISSING_COMMA,                /* Two entries have no comma between. */
    INVALID_NUMBER                /* An entry have an invalid number, maybe an ASCII character is placed. */
};

struct cjlib_json_error
{
    char *c_property_name;                    /* The name of the property in which the error occurred. */
    char *c_property_value;                   /* The value of the property in which the error occurred. */
    enum cjlib_json_error_types c_error_code; /* A error code indicating the error. */
};

/**
 * cjlib_json_error_init Initializes the internal error structure.
 *
 * @return An integer indicating whether the initialization were successfully. On success 0 is returend,
 * otherwise -1.
 */
extern int cjlib_json_error_init(void);

/**
 * cjlib_json_error_destroy Free's the memory allocated by the internal error structure.
 */
extern void cjlib_json_error_destroy(void);

/**
 * cjilb_setup_error creates a new error.
 *
 * @param property_name The name of the property that cause the error, if available.
 * @param property_value The value of the property, if available.
 * @param error_code Specifies which of the error listed in cjlib_json_error_types struct can represent the error
 * with more detail.
 */
extern void cjlib_setup_error
(const char *property_name, const char *property_value,
 enum cjlib_json_error_types error_code);

/**
 * cjlib_json_get_error Retrieves the error stored in the internal
 * error structure.
 *
 * @param dst A pointer to the memory area to copy the internal error.
 */
extern void cjlib_json_get_error(struct cjlib_json_error *restrict dst);

/**
 * cjlib_error_indicator_correction corrects a lost error.
 *
 * @param func_err_code The error, if any, produced by some random function.
 * @return An error from the list of errors.
 */
extern enum cjlib_json_error_types cjlib_error_indicator_correction(int func_err_code);

#endif
