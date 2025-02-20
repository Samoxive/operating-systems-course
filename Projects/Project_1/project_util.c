#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "c_string.h"
#include "project_util.h"

COLOR get_color_from_string(char* str) {
    if (c_string_equals(str, "red")) {
        return RED;
    } else if (c_string_equals(str, "green")) {
        return GREEN;
    } else if (c_string_equals(str, "blue")) {
        return BLUE;
    } else if (c_string_equals(str, "orange")) {
        return ORANGE;
    } else if (c_string_equals(str, "yellow")) {
        return YELLOW;
    } else if (c_string_equals(str, "white")) {
        return WHITE;
    } else if (c_string_equals(str, "black")) {
        return BLACK;
    } else if (c_string_equals(str, "purple")) {
        return PURPLE;
    } else {
        return INVALID_COLOR;
    }
}

char* get_string_from_color(COLOR color) {
    switch (color) {
        case RED:
            return "red";
        case GREEN:
            return "green";
        case BLUE:
            return "blue";
        case ORANGE:
            return "orange";
        case YELLOW:
            return "yellow";
        case WHITE:
            return "white";
        case BLACK:
            return "black";
        case PURPLE:
            return "purple";
        default:
            return "invalid_color";
    }
}

color_parse_result parse_string_into_colors(char* string) {
    color_parse_result result = {0};
    char buf[128] = {0};
    i32 string_length = strlen(string);
    FILE* string_buffer = fmemopen(string, string_length * sizeof(char), "r");

    while (fscanf(string_buffer, " %[^,\n],", buf) != EOF) {
        COLOR color = get_color_from_string(buf);

        switch (color) {
            case RED:
                result.red += 1;
                break;
            case GREEN:
                result.green += 1;
                break;
            case BLUE:
                result.blue += 1;
                break;
            case ORANGE:
                result.orange += 1;
                break;
            case YELLOW:
                result.yellow += 1;
                break;
            case WHITE:
                result.white += 1;
                break;
            case BLACK:
                result.black += 1;
                break;
            case PURPLE:
                result.purple += 1;
                break;
            case INVALID_COLOR:
                continue;
        }
    }

    fclose(string_buffer);
    return result;
}

color_list split_input_into_colors(char* string) {
    i32 i = 0;
    i32 capacity = 8;
    COLOR* colors = malloc(capacity * sizeof(COLOR));
    char buf[128] = {0};
    i32 string_length = strlen(string);
    FILE* string_buffer = fmemopen(string, string_length * sizeof(char), "r");
    while (fscanf(string_buffer, " %[^,\n],", buf) != EOF) {
        COLOR color = get_color_from_string(buf);

        if (i == capacity) {
            capacity *= 2;
            colors = realloc(colors, capacity * sizeof(COLOR));
        }

        colors[i] = color;
        i++;
    }

    fclose(string_buffer);
    color_list list = {.colors = colors, .size = i};
    return list;
}

void write_final_output_to_file(COLOR target_color,
                                i32 result_count,
                                color_parse_result* results,
                                char** file_names,
                                char* output_file_name) {
    FILE* output_file = fopen(output_file_name, "w");
    if (output_file == null) {
        printf("Could not open output file.\n");
        exit(-1);
    }

    for (i32 i = 0; i < result_count; i++) {
        i32 target_color_count = 0;
        i32 total_colors_count = 0;
        color_parse_result result = results[i];
        switch (target_color) {
            case RED:
                target_color_count += result.red;
                break;
            case GREEN:
                target_color_count += result.green;
                break;
            case BLUE:
                target_color_count += result.blue;
                break;
            case ORANGE:
                target_color_count += result.orange;
                break;
            case YELLOW:
                target_color_count += result.yellow;
                break;
            case WHITE:
                target_color_count += result.white;
                break;
            case BLACK:
                target_color_count += result.black;
                break;
            case PURPLE:
                target_color_count += result.purple;
                break;
            case INVALID_COLOR:
                continue;
        }

        total_colors_count += results[i].red + results[i].green +
                              results[i].blue + results[i].orange +
                              results[i].yellow + results[i].white +
                              results[i].black + results[i].purple;
        fprintf(output_file,
                "%s: %d/%d\n%s, black: %d, blue: %d, green: %d, orange: %d, "
                "purple: %d, "
                "red: %d, white: %d, yellow: %d\n",
                get_string_from_color(target_color), target_color_count,
                total_colors_count, file_names[i], result.black, result.blue,
                result.green, result.orange, result.purple, result.red,
                result.white, result.yellow);
    }

    fclose(output_file);
}

char** extract_input_files_names_from_argv(char** argv, i32 argc) {
    i32 names_length = argc - 4;
    char** names = c_string_array_subarray(argv, 3, argc - 1);
    c_string_sort_array(names, names_length);
    if (c_string_array_has_duplicates(names, names_length)) {
        printf("Input file names must be unique.\n");
        exit(-1);
    }

    return names;
}

i32 get_index_from_pid_array(pid_t* pids, i32 pid_length, pid_t target_pid) {
    for (i32 i = 0; i < pid_length; i++) {
        if (pids[i] == target_pid) {
            return i;
        }
    }

    printf("Received invalid pid from slave process.\n");
    exit(-1);
}