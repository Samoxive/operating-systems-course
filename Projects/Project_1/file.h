#pragma once

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "c_string.h"

char* read_file_to_string(char*);
bool write_struct_to_file(char* file_name, void* data, i32 size);
bool read_struct_from_file(char* file_name, void* data, i32 size);
