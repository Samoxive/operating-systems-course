#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

const char* SHARED_FILE_NAME = "./psearch2a.tmp";

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
    FILE* shared_file = fopen(SHARED_FILE_NAME, "ab");
    if (shared_file == null) {
        printf("Failed to open shared shared_file.\n");
        exit(-1);
    }

    i32 shared_file_fd = fileno(shared_file);
    flock(shared_file_fd, LOCK_EX);
    fwrite(&pid_result, sizeof(pid_color_parse_result), 1, shared_file);
    flock(shared_file_fd, LOCK_UN);

    fclose(shared_file);
    free(file_content);
    return 0;
}