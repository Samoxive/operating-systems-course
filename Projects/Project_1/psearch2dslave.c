#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

const char* FIFO_PIPE_NAME = "./psearch2d.tmp";

i32 main(i32 argc, char** argv) {
    if (argc != 3) {
        printf("Invalid command line input.\n");
        exit(-1);
    }

    char* input_file_name = argv[2];
    char* file_content = read_file_to_string(input_file_name);
    if (file_content == null) {
        printf("Could not read given file.\n");
        exit(-1);
    }

    color_parse_result result = parse_string_into_colors(file_content);
    pid_color_parse_result pid_result = {.pid = getpid(), .result = result};
    i32 fifo_fd = open(FIFO_PIPE_NAME, O_RDWR);
    if (write(fifo_fd, &pid_result, sizeof(pid_color_parse_result)) == -1) {
        printf("Could not write to FIFO file.\n");
        exit(-1);
    }

    close(fifo_fd);
    free(file_content);

    return 0;
}