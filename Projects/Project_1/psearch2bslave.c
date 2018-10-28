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

const char* psearch2b_shm_name = "/psearch2b_shm";
const char* psearch2b_semaphore_name = "/psearch2b_semaphore";

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

    i32 fd_results = shm_open(psearch2b_shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd_results == -1) {
        printf("Could not open shared memory block.\n");
        exit(-1);
    }

    struct stat shm_stats = {0};
    if (fstat(fd_results, &shm_stats) == -1) {
        printf("Failed to get size of shared memory object.\n");
        exit(-1);
    }

    pid_color_parse_result* pid_results =
        mmap(null, shm_stats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
             fd_results, 0);
    if (pid_results == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    sem_t* semaphore = sem_open(psearch2b_semaphore_name, O_RDWR);
    if (semaphore == SEM_FAILED) {
        printf("Could not open semaphore.\n");
        exit(-1);
    }

    if (sem_wait(semaphore) == -1) {
        printf("Could not wait on semaphore.\n");
        exit(-1);
    }

    i32 input_files_count = shm_stats.st_size / sizeof(pid_color_parse_result);
    bool is_stored = false;
    for (i32 i = 0; i < input_files_count; i++) {
        if (pid_results[i].pid == 0) {
            pid_results[i] = pid_result;
            is_stored = true;
            break;
        }
    }

    if (sem_post(semaphore) == -1) {
        printf("Could not post on semaphore.\n");
        exit(-1);
    }

    if (!is_stored) {
        printf("Failed to store result on shared memory.\n");
        exit(-1);
    }

    sem_close(semaphore);
    munmap(pid_results, input_files_count * sizeof(pid_color_parse_result));
    free(file_content);
    return 0;
}