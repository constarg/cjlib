[![CJLIB](https://github.com/constarg/cjlib/actions/workflows/cjlib.yml/badge.svg?branch=main)](https://github.com/constarg/cjlib/actions/workflows/cjlib.yml)

# Introduction
Cjlib is a library leveraged for accessing or/and modifying the contents of files following the JSON standard. It provides a set of functions to easily make any process into a JSON file with as much efficiency as possible.

# Installation Requirements

Before building the cjlib, make sure to install the necessary packages:

```bash
sudo apt-get install build-essential git gcc
```

# Download & Build

Get started by cloning the repository and navigating to the cjlib folder:
```
git clone https://github.com/constarg/cjlib.git
cd cjlib/
```
Now, build the library using the following command:

```
make
```

# Debugging
If any issue with the library is noticed, don't hesitate to contact me using either my email (constarg@pm.me) or by opening an issue in the GitHub repository (the second option is preferable). Please, to make the process easier, I would like to ask you to first compile the library using the following command and also, if possible, send the core dump contents with the bug report. Thanks!
```
make debug
```

# Doc
The documentation is located in the current repository within the 'doc/' folder. Any question about the documentation of the libary is welcome! please contact with me using either my email (constarg@pm.me) or by opening an issue in the current repository.

# Usage example
Following, there is an illustrative example of how to use the cjlib to make a simple operation, to read the **student_name** of the provided JSON. 

The provided JSON is the following:

```JSON
{
	"student_name": "John",
	"student_id": 1234
}
```

While to access the student_name, the following code is required:

```C
#include <stdio.h>
#include "cjlib.h"

int main(void) 
{
	struct cjlib_json json_file;
	struct cjlib_json_data dst;

	cjlib_json_init(&json_file);
	cjlib_json_data_init(&dst);
	
	// Open the JSON file.
	if (-1 == cjlib_json_open(&json_file, "input/example.json", "r")) {
		(void) printf("Failed to open the json file\n");
		exit(-1);
	}
	
	// Read the contents.
	if (-1 == cjlib_json_read(&json_file)) {
		(void) printf("Failed to read the json contents");
		cjlib_json_close(&json_file);
		exti(-1);
	}
	
	// Try to get, if exists, the contents of the field with key: student_name.
	if (-1 == cjlib_json_get(&dst, &json_file, "student_name")) {
		(void) printf("Error\n");
		cjlib_json_close(&json_file);
		exit(-1);
	}
	
	// Print the contents of the field with key: student_name.
	(void) printf("%s\n", dst.c_value.c_str);
	
	// Free the memory.
	cjlib_json_close(&json_file);
}
```

More ilustrative example could be found in the **test/** directory in the folder tree. 

# Contributors

<a href="https://github.com/constarg/cjlib/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=constarg/cjlib" />
</a>
