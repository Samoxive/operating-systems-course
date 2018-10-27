#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

typedef struct {
    i32 index;
    color_parse_result result;
} piped_color_parse_result;

typedef struct {
    i32 fd_pipe_r;
    i32 fd_pipe_w;
} pipe_fd_pair;

void child_process(char* file_name, i32 fd_pipe_w, i32 index) {
    char* file_content = read_file_to_string(file_name);
    color_parse_result result = parse_string_into_colors(file_content);
    piped_color_parse_result piped_result = {.index = index, .result = result};
    if (write(fd_pipe_w, &piped_result, sizeof(piped_color_parse_result)) ==
        -1) {
        printf("Could not write to pipe.");
        exit(-1);
    }
    free(file_content);
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

    i32 input_files_count = argc - 4;
    char* output_file_name = argv[argc - 1];
    char** input_files_names = c_string_array_subarray(argv, 3, argc - 1);
    i32 pipe_fds[2] = {0};
    if (pipe(pipe_fds) == -1) {
        printf("Could not create pipe.");
        exit(-1);
    }
    i32 fd_pipe_r = pipe_fds[0];
    i32 fd_pipe_w = pipe_fds[1];

    for (i32 i = 0; i < input_files_count; i++) {
        i32 f = fork();
        if (f == 0) {
            child_process(input_files_names[i], fd_pipe_w, i);
            close(fd_pipe_r);
            close(fd_pipe_w);
            free(input_files_names);
            exit(0);
            break;
        }
    }

    color_parse_result* results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        piped_color_parse_result result = {0};
        if (read(fd_pipe_r, &result, sizeof(piped_color_parse_result)) == -1) {
            printf("Could not read from pipe.");
            exit(-1);
        }
        results[result.index] = result.result;
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);
    close(fd_pipe_r);
    close(fd_pipe_w);
    free(results);
    free(input_files_names);
    return 0;
}