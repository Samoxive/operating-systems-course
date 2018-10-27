#include <stdio.h>
#include "common_include.h"

i32 main(void) {
    char* content = read_file_to_string("../input.txt");
    printf("%s\n", content);
    parse_string_into_colors(content);
    free(content);
    return 0;
}