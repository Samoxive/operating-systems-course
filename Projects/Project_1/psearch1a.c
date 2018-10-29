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
    for (i32 i = 0; i < input_files_count; i++) {
        i32 f = fork();
        if (f == 0) {
            child_process(input_files_names[i]);
            break;
        }
    }

    i32 wstatus = 0;
    while (wait(&wstatus) > 0)
        ;

    color_parse_result* output_results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        char* temp_output_file_name = c_string_concat(input_files_names[i], ".tmp");
        read_struct_from_file(temp_output_file_name, &(output_results[i]),
                              sizeof(color_parse_result));
        free(temp_output_file_name);
    }

    write_final_output_to_file(target_color, input_files_count, output_results,
                               input_files_names, output_file_name);
    free(output_results);
    free(input_files_names);
    return 0;
}
