# JSON
## JSON types
The JSON standard features a plethora of types, like **strings**, **booleans**, **numbers** (both floating points and integers), and **null**. Moreover, it features some more complex data structures such as **arrays** and **objects**. To accommodate this, cjlib has implemented the following enum, which is used to differentiate between the forenamed types and data structures.

```C
enum cjlib_json_datatypes
{
    CJLIB_STRING,
    CJLIB_NUMBER,
    CJLIB_ARRAY,
    CJLIB_BOOLEAN,
    CJLIB_OBJECT,
    CJLIB_NULL
};
```

The user may use this enum with some other elements that will be discussed in the following sections, to perform various manipulations on the JSON contents.

`File: cjlib.h`

## JSON structures
To group all the information required for the JSON to be represented in the memory, cjlib has implemented a structure called **cjlib_json** which is defined as follows

```C
struct cjlib_json
{
    cjlib_json_fd c_fp;        // The file pointer of the json file.
    cjlib_json_object *c_dict; // The dictionary that contains the values of the json.
    char *c_path;              // The path to the respective file.
};
```

`The user should not modify the contents of this structure in the code, as it may break the internal operation of the library, leading to memory leaks and memory corruption. Only the library functions must have the right to modify the contents of this structure. Where it is required, the user may access, it for read purposes only (of course I would like to make the declaration of the structure private, but in this case, it is less flexible and requires more memory allocations and systemcalls, therefore is less efficient in this way).`

`File: cjlib.h`

# Errors

## Error types
The library provides the user with a plethora of errors for the correct error handling. To do this, it provides the following types of errors that may appear when manipulating JSON files.

``` C
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

```

`File: cjlib_error.h`

## Error structure
To help discover the cause of an error and therefore handle it with the appropriate actions, cjlib provides the following structure with as much information as possible for the error that is caused. In some cases, some fields of this structure may be left empty, because the internal function builds the errors while parsing or editing the JSON file. Depending on the state where the error occurred, more or less information may be available about the error.

```C
struct cjlib_json_error
{
    char *c_property_name;                    // The name of the property in which the error occurred.
    char *c_property_value;                   // The value of the property in which the error occurred.
    enum cjlib_json_error_types c_error_code; // A error code indicating the error.
};

```
`File: cjlib_error.h`

# Error related functions
Cjlib provides the errors by manipulating an internal (private) variable in the implementation of cjlib_error.c file. This is done to protect it from mistaken actions. To access the private variable that has the information about the error that occurred, the library provides the following functions. Each of the following functions is **thread safe**, as it uses mutex, to allow multiple threads to check for errors (if this is necessary).

___
## cjlib_json_error_init
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_error_destroy
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_setup_error
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_get_error
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_error_indicator_correction
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```


# JSON related Functions
___
## cjlib_json_init
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_destroy
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_make_object
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```


___
## cjlib_json_data_init
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_free_array
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_data_destroy
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_array_free_data
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_make_array
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_array_append
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_array_get
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_object_set
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_object_get
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_object_remove
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

## Example
```C
```

___
## cjlib_json_set
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_get
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_remove
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_open
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_close
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_read
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_object_stringtify
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_stringtify
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```

___
## cjlib_json_dump
___
### Description

### Function signature
```C

```

### Parameters

### Return
--

### Errors
--

### Example
```C
```