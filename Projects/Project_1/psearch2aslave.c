#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

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
    char* output_shm_name = c_string_concat("/psearch2a_", input_file_name);
    for (i32 i = 1; i < strlen(output_shm_name); i++) {
        if (output_shm_name[i] == '/') {
            output_shm_name[i] = '.';
        }
    }

    i32 shm_fd = shm_open(output_shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        printf("Could not open shared memory blocks.\n");
        exit(-1);
    }

    color_parse_result* shm_result =
        mmap(null, sizeof(color_parse_result), PROT_READ | PROT_WRITE,
             MAP_SHARED, shm_fd, 0);
    if (shm_result == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    *shm_result = result;

    munmap(shm_result, sizeof(color_parse_result));
    shm_unlink(output_shm_name);
    free(file_content);
    free(output_shm_name);
    exit(0);
}