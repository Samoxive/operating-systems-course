#include <stdio.h>
#include "common_include.h"

i32 main(i32 argc, char** argv) {
    char* content = read_file_to_string("../input.txt");
    printf("%s\n", content);
    color_parse_result result = parse_string_into_colors(content);
    free(content);
    return 0;
}