#include <stdlib.h>
#include <string.h>
#include "typedefs.h"

char* c_string_concat(char* a, char* b) {
    i32 length = strlen(a) + strlen(b) + 1;
    char* new_string = malloc(length * sizeof(char));
    strcpy(new_string, a);
    strcat(new_string, b);
    return new_string;
}

bool c_string_equals(char* a, char* b) {
    return strcmp(a, b) == 0;
}

char** c_string_array_subarray(char** arr, i32 start, i32 end) {
    i32 length = end - start;
    char** new_arr = malloc(length * sizeof(char*));
    for (i32 i = 0; i < length; i++) {
        new_arr[i] = arr[start + i];
    }

    return new_arr;
}

int c_string_compare(const void* a, const void* b) {
    return strcmp(*((char**)a), *((char**)b));
}

void c_string_sort_array(char** arr, i32 length) {
    qsort(arr, length, sizeof(char*), c_string_compare);
}

// only pass sorted array
bool c_string_array_has_duplicates(char** arr, i32 length) {
    for (i32 i = 1; i < length; i++) {
        if (strcmp(arr[i - 1], arr[i]) == 0) {
            return true;
        }
    }

    return false;
}