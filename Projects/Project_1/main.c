#include <stdio.h>
#include "file.h"

int main(int argc, char** argv) {
    char* content = read_file_to_string("../main.c");
    printf("%s\n", content);
    return 0;
}