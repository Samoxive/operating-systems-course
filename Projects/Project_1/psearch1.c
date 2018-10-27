#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

void child_process(char* file_name) {
    char* file_content = read_file_to_string(file_name);
    color_parse_result result = parse_string_into_colors(file_content);
    char* output_file_name = c_string_concat(file_name, ".tmp");
    write_struct_to_file(output_file_name, &result, sizeof(result));
    free(file_content);
    free(output_file_name);
    exit(0);
}

i32 main(i32 argc, char** argv) {
    // psearch orange 3 input1.txt input2.txt input3.txt output.txt
    if (argc < 5) {
        printf("Invalid command line input.");
        return -1;
    }

    char* target_color_string = argv[1];
    COLOR target_color = get_color_from_string(target_color_string);
    if (target_color == INVALID_COLOR) {
        printf("Invalid color input.");
        exit(-1);
    }

    char* output_file_name = argv[argc - 1];
    for (i32 i = 3; i < argc - 1; i++) {
        i32 f = fork();
        if (f == 0) {
            child_process(argv[i]);
            break;
        }
    }

    i32 wstatus = 0;
    while (wait(&wstatus) > 0)
        ;

    i32 input_files_count = argc - 4;
    color_parse_result* output_results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        char* output_file_name = c_string_concat(argv[3 + i], ".tmp");
        read_struct_from_file(output_file_name, &(output_results[i]),
                              sizeof(color_parse_result));
        free(output_file_name);
    }

    char** input_files_names = c_string_array_subarray(argv, 3, argc - 1);
    write_final_output_to_file(target_color, argc - 4, output_results,
                               input_files_names, output_file_name);
    free(output_results);
    free(input_files_names);
    return 0;
}