#pragma once

#include <sys/types.h>
#include "typedefs.h"

typedef enum {
    RED,
    GREEN,
    BLUE,
    ORANGE,
    YELLOW,
    WHITE,
    BLACK,
    PURPLE,
    INVALID_COLOR,
} COLOR;

typedef struct {
    COLOR* colors;
    i32 size;
} color_list;

typedef struct {
    i32 red;
    i32 green;
    i32 blue;
    i32 orange;
    i32 yellow;
    i32 white;
    i32 black;
    i32 purple;
} color_parse_result;

typedef struct {
    pid_t pid;
    color_parse_result result;
} pid_color_parse_result;

COLOR get_color_from_string(char*);
char* get_string_from_color(COLOR);
color_parse_result parse_string_into_colors(char* string);
color_list split_input_into_colors(char*);
void write_final_output_to_file(COLOR target_color,
                                i32 result_count,
                                color_parse_result* results,
                                char** file_names,
                                char* output_file_name);
char** extract_input_files_names_from_argv(char** argv, i32 argc);
i32 get_index_from_pid_array(pid_t* pids, i32 pid_length, pid_t target_pid);