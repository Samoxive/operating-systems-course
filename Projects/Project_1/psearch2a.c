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

const char* psearch2aslave_exe = "./psearch2aslave";
const char* psearch2a_shared_file_name = "./psearch2a.tmp";

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
    FILE* shared_file = fopen(psearch2a_shared_file_name, "wb+");
    if (shared_file == null) {
        printf("Failed to open shared file.\n");
        perror("");
        exit(-1);
    }

    i32 shared_file_fd = fileno(shared_file);
    pid_t* slave_pids = malloc(input_files_count * sizeof(pid_t));
    for (i32 i = 0; i < input_files_count; i++) {
        pid_t f = fork();
        if (f == 0) {
            // exec* family functions don't give a damn about acting reasonable,
            // given argv array will be passed to the slave directly, so to
            // emulate normal behavior, we give the slave executable's path to
            // argv as well. An hour was wasted on this.
            execl(psearch2aslave_exe, psearch2aslave_exe, target_color_string,
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

    pid_color_parse_result* pid_results =
        mmap(null, input_files_count * sizeof(pid_color_parse_result),
             PROT_READ, MAP_SHARED, shared_file_fd, 0);
    if (pid_results == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }
    color_parse_result* results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        i32 index = get_index_from_pid_array(slave_pids, input_files_count,
                                             pid_results[i].pid);
        results[index] = pid_results[i].result;
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);
    munmap(pid_results, input_files_count * sizeof(pid_color_parse_result));
    fclose(shared_file);
    remove(psearch2a_shared_file_name);
    free(results);
    free(input_files_names);
    free(slave_pids);
    return 0;
}
