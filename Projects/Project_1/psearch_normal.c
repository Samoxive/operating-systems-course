#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_include.h"

i32 main(i32 argc, char** argv) {
    // psearch orange 3 input1.txt input2.txt input3.txt output.txt
    if (argc < 5) {
        printf("Invalid command line input.\n");
        exit(-1);
    }

    char* target_color_string = argv[1];
    COLOR target_color = get_color_from_string(target_color_string);
    if (target_color == INVALID_COLOR) {
        printf("Invalid color input.\n");
        exit(-1);
    }

    i32 input_files_count = argc - 4;
    char* output_file_name = argv[argc - 1];
    char** input_files_names = extract_input_files_names_from_argv(argv, argc);
    color_parse_result* results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        char* file_content = read_file_to_string(input_files_names[i]);
        if (file_content == null) {
            printf("Could not read given file.\n");
            exit(-1);
        }

        color_parse_result result = parse_string_into_colors(file_content);
        results[i] = result;
        free(file_content);
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);
    free(results);
    free(input_files_names);
    return 0;
}
