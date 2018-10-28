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

const char* psearch2a_shm_name = "/psearch";

i32 main(i32 argc, char** argv) {
    if (argc != 5) {
        printf("Invalid command line input.\n");
        exit(-1);
    }

    char* input_file_name = argv[2];
    i32 result_index = atoi(argv[3]);
    i32 result_length = atoi(argv[4]);
    printf("%d %d\n", result_index, result_length);
    if (result_index < 0 || result_length < 0) {
        printf("Invalid command line input.\n");
        exit(-1);
    }
    char* file_content = read_file_to_string(input_file_name);
    if (file_content == null) {
        printf("Could not read given file.\n");
        exit(-1);
    }

    color_parse_result result = parse_string_into_colors(file_content);

    i32 shm_fd = shm_open(psearch2a_shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        char s[100];
        printf("Could not open shared memory blocks.\n");
        exit(-1);
    }

    color_parse_result* shm_result =
        mmap(null, result_length * sizeof(color_parse_result),
             PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_result == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    shm_result[result_index] = result;

    munmap(shm_result, result_length * sizeof(color_parse_result));
    free(file_content);
    exit(0);
}