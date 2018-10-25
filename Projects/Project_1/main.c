#include <stdio.h>
#include "common_include.h"

int main(int argc, char** argv) {
    char* content = read_file_to_string("../input.txt");
    printf("%s\n", content);
    color_list list = split_input_into_colors(content);
    for (int i = 0; i < list.size; i++) {
        printf("%s\n", get_string_from_color(list.colors[i]));
    }

    free(content);
    free(list.colors);
    return 0;
}