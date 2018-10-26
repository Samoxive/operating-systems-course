#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        case INVALID_COLOR:
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