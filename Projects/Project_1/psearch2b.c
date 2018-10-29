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

const char* SHM_SEMAPHORES = "/semaphores";
const char* SHM_RESULTS = "/results";
const char* SLAVE_EXE = "./psearch2bslave";

i32 main(i32 argc, char** argv) {
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

    i32 fd_results = shm_open(SHM_RESULTS, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd_results == -1) {
        printf("Could not open shared memory block.\n");
        exit(-1);
    }

    if (ftruncate(fd_results,
                  input_files_count * sizeof(pid_color_parse_result)) == -1) {
        printf("Could not extend shared memory block.\n");
        exit(-1);
    }

    pid_color_parse_result* pid_results =
        mmap(null, input_files_count * sizeof(pid_color_parse_result),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd_results, 0);
    if (pid_results == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    sem_t* semaphore =
        sem_open(SHM_SEMAPHORES, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (semaphore == SEM_FAILED) {
        printf("Could not create semaphore.\n");
        exit(-1);
    }

    pid_color_parse_result pid_result_dummy = {0};
    for (i32 i = 0; i < input_files_count; i++) {
        pid_results[i] = pid_result_dummy;
    }

    pid_t* slave_pids = malloc(input_files_count * sizeof(pid_t));
    for (i32 i = 0; i < input_files_count; i++) {
        pid_t f = fork();
        if (f == 0) {
            execl(SLAVE_EXE, SLAVE_EXE, target_color_string,
                  input_files_names[i], null);

            printf("Slave process failed. Errno: %d\n", errno);
            exit(-1);
        } else {
            slave_pids[i] = f;
        }
    }

    i32 wstatus = 0;
    while (wait(&wstatus) > 0)
        ;

    color_parse_result* results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        i32 index = get_index_from_pid_array(slave_pids, input_files_count,
                                             pid_results[i].pid);
        results[index] = pid_results[i].result;
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);

    sem_close(semaphore);
    munmap(pid_results, input_files_count * sizeof(pid_color_parse_result));
    shm_unlink(SHM_SEMAPHORES);
    free(results);
    free(input_files_names);
    free(slave_pids);
    return 0;
}