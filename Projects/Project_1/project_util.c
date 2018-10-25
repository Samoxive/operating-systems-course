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

color_list split_input_into_colors(char* string) {
    int i = 0;
    int capacity = 8;
    COLOR* colors = malloc(capacity * sizeof(COLOR));
    char buf[128] = {0};
    i32 string_length = strlen(string);
    FILE *string_buffer = fmemopen(string, string_length * sizeof(char), "r");
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