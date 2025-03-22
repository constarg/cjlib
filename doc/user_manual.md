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

## cjlib_json
To group all the information required for the JSON to be represented in the memory, cjlib has implemented a structure called **cjlib_json** which is defined as follows

```C
struct cjlib_json
{
    cjlib_json_fd c_fp;        // The file pointer of the json file.
    cjlib_json_object *c_dict; // The dictionary that contains the values of the json.
    char *c_path;              // The path to the respective file.
};
```

`The user should not modify the contents of this structure in the code, as it may break the internal operation of the library, leading to memory leaks and memory corruption. Only the library functions must have the right to modify the contents of this structure. If it is required, the user may access it for read purposes only (of course I would like to make the declaration of the structure private, but in this case, it is less flexible and requires more memory allocations and systemcalls, therefore is less efficient in this way).`

`File: cjlib.h`

## cjlib_json_data
The most important structure for the library user is the **cjlib_json_data** structure. This structure consists of the information needed to save the required data to the internal data structure of the cjlib. The structure is defined as shown below:

```C
struct cjlib_json_data
{
    union cjlib_json_data_disting c_value; // The data to store/retrieve.
    enum cjlib_json_datatypes c_datatype;  // The type of the data.
};
```

This structure consists of two elements. The first is the **c_value** which represents the JSON data to be saved/retrieved (boolean, number, object, arrays). The second represents the type of data stored in the **c_value** member. The **c_datatype** member will not be discussed further, as the **Json types** section described the available types that could be stored in this member. The member to which the focus will be pointed is the c_value member. As shown in the definition of the structure, this field/member is another built-in data structure of C, which is a union, that helps to distinguish between the available types of the JSON, using only one single and simple data structure. Shown below is the definition of the union that represents the data of the JSON.

```C
union cjlib_json_data_disting
{
    char *c_str;               // string
    cjlib_json_num c_num;      // Number
    cjlib_json_bool c_boolean; // Boolean.
    cjlib_json_object *c_obj;  // json object.
    void *c_null;              // null
    cjlib_json_array *c_arr;   // array.
};

```

The user may use both the cjlib_json_data structure and the cjlib_json_data_disting union to determine 1) the type of the data that was stored/retrieved from the internal representation of the JSON in the memory and 2) to receive/store the respective value from/in the corresponding field of the union. Following, there is an illustrative example that creates simple data, a number, using the definitions presented here.

```C
#include "cjlib.h"

int main(void) {
	struct cjlib_json_data my_data; // Where the data are stored.
	
	my_data.c_datatype = CJLIB_NUMBER; // The type of the data.
	my_data.c_value    = 50;  	   // The actual data.
	// store the data in the internal data structure...
}
```

Further details on how to store/retrieve the data to/from the internal data structure are described in the following sections.


`File: cjlib.h`

# Errors

## Error types
The library provides the user with a plethora of errors for error handling. To do this, it provides the following types of errors that may appear when manipulating JSON files.

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
## cjlib_json_get_error
___
### Description
The cjlib_json_get error is leveraged to request the error produced by a function. It is typically used after a function fails to get more details about the error that occurred internally in the library. Each of the library functions returns a subset of the available errors; thus, there is no need to check for all the errors for each function. The possible errors from each library function are documented.

The user may ignore the errors retrieved from the internal error structure but must always check if a library function is erroneous. Otherwise, unexpected behavior may occur, breaking the program.

### Function signature
```C
void cjlib_json_get_error(struct cjlib_json_error *restrict dst);
```

### Parameters
**dst**: A pointer to the memory region to store the details about the error occured.

### Return
--

### Errors
--

### Example
```C
#include "cjlib_error.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {	
	// Oh!, an error occured during a hypithetical function from the library.
	// Try to get details on where and why the error were occured.
	
	struct cjlib_json_error error_details;
	cjlib_json_get_error(&error_details);
	
	// OK, lets see if the library have some more helpful details, like which element caused the error.
	
	if (NULL != error_details.c_property_name) {
		(void) printf("Errornus propery name: %s\n", error_details.c_property_name);
	}
	
	// Lets see if we can retrieve the value of the property.
	if (NULL != error_details.c_property_value) {
		(void) printf("Errornus property value: %s\n", error_details.c_property_value);
	}
	
	switch (error_details.c_error_code) {
		case INVALID_TYPE: 
			(void) printf("Ah!, there is a type that is not valid for the JSON standard!\n")
			break;
		case MISSING_SEPERATOR:
			(void) printf("Somewhere in the the JSON file, there must be a separator: but there is not\n")
		
		...
	}

	return EXIT_SUCCESS
}

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
