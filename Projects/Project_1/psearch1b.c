#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

const char* SHM_NAME_SEMAPHORES = "/semaphores";
const char* SHM_NAME_RESULTS = "/results";

void child_process(char* file_name,
                   i32 fd_semaphores,
                   i32 fd_results,
                   i32 index,
                   i32 input_files_count) {
    char* file_content = read_file_to_string(file_name);
    color_parse_result result = parse_string_into_colors(file_content);
    sem_t* semaphores =
        mmap(null, input_files_count * sizeof(sem_t), PROT_READ | PROT_WRITE,
             MAP_SHARED, fd_semaphores, 0);
    color_parse_result* results =
        mmap(null, input_files_count * sizeof(color_parse_result),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd_results, 0);
    if (semaphores == MAP_FAILED || results == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    results[index] = result;
    if (sem_post(&(semaphores[index])) == -1) {
        printf("Could not post on semaphore.\n");
        exit(-1);
    };

    munmap(semaphores, input_files_count * sizeof(sem_t));
    munmap(results, input_files_count * sizeof(color_parse_result));
    free(file_content);
}

i32 main(i32 argc, char** argv) {
    // psearch orange 3 input1.txt input2.txt input3.txt output.txt
    if (argc < 5) {
        printf("Invalid command line input.\n");
        exit(-1);
    }

    char* target_color_string = argv[1];
    COLOR target_color = get_color_from_string(target_color_string);
    if (target_color == INVALID_COLOR) {
        printf("Invalid color input.\n");
        exit(-1);
    }

    i32 input_files_count = argc - 4;
    char* output_file_name = argv[argc - 1];
    char** input_files_names = extract_input_files_names_from_argv(argv, argc);
    i32 fd_semaphores =
        shm_open(SHM_NAME_SEMAPHORES, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    i32 fd_results =
        shm_open(SHM_NAME_RESULTS, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd_semaphores == -1 || fd_results == -1) {
        printf("Could not open shared memory blocks.\n");
        exit(-1);
    }

    if (ftruncate(fd_semaphores, input_files_count * sizeof(sem_t)) == -1 ||
        ftruncate(fd_results, input_files_count * sizeof(color_parse_result)) ==
            -1) {
        printf("Could not extend shared memory blocks.\n");
        exit(-1);
    }

    sem_t* semaphores =
        mmap(null, input_files_count * sizeof(sem_t), PROT_READ | PROT_WRITE,
             MAP_SHARED, fd_semaphores, 0);
    color_parse_result* results =
        mmap(null, input_files_count * sizeof(color_parse_result),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd_results, 0);
    if (semaphores == MAP_FAILED || results == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    for (i32 i = 0; i < input_files_count; i++) {
        if (sem_init(&(semaphores[i]), 1, 0) == -1) {
            printf("Could not create semaphore.\n");
            exit(-1);
        }
    }

    for (i32 i = 0; i < input_files_count; i++) {
        i32 f = fork();
        if (f == 0) {
            child_process(input_files_names[i], fd_semaphores, fd_results, i,
                          input_files_count);
            free(input_files_names);
            exit(0);
        }
    }

    for (i32 i = 0; i < input_files_count; i++) {
        if (sem_wait(&(semaphores[i])) == -1) {
            printf("Could not wait on semaphore.\n");
            exit(-1);
        }
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);
    for (i32 i = 0; i < input_files_count; i++) {
        sem_destroy(&(semaphores[i]));
    }
    munmap(semaphores, input_files_count * sizeof(sem_t));
    munmap(results, input_files_count * sizeof(color_parse_result));
    shm_unlink(SHM_NAME_SEMAPHORES);
    shm_unlink(SHM_NAME_RESULTS);
    free(input_files_names);
    return 0;
}