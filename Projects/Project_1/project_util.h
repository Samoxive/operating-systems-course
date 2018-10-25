#pragma once

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
    int size;
} color_list;

COLOR get_color_from_string(char*);
char* get_string_from_color(COLOR);
color_list split_input_into_colors(char*);