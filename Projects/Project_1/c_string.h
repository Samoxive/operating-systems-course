#pragma once

#include <stdlib.h>
#include <string.h>
#include "typedefs.h"

char* c_string_concat(char*, char*);
bool c_string_equals(char*, char*);
char** c_string_array_subarray(char** arr, i32 start, i32 end);
