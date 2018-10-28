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

const char* psearch2aslave_exe = "./build/psearch2aslave";
const char* psearch2a_shm_name = "/psearch";

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
    char** input_files_names = c_string_array_subarray(argv, 3, argc - 1);
    i32 shm_fd =
        shm_open(psearch2a_shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        printf("Could not open shared memory blocks.\n");
        exit(-1);
    }
    if (ftruncate(shm_fd, input_files_count * sizeof(color_parse_result)) ==
        -1) {
        printf("Could not extend shared memory blocks.\n");
        exit(-1);
    }

    color_parse_result* results =
        mmap(null, input_files_count * sizeof(color_parse_result),
             PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (results == MAP_FAILED) {
        printf("Could not mmap shared memory blocks.\n");
        exit(-1);
    }

    for (i32 i = 0; i < input_files_count; i++) {
        i32 f = fork();
        if (f == 0) {
            char index_str[12];
            char length_str[12];
            sprintf(index_str, "%d", i);
            sprintf(length_str, "%d", input_files_count);
            // exec* family functions don't give a damn about acting reasonable,
            // given argv array will be passed to the slave directly, so to
            // emulate normal behavior, we give the slave executable's path to
            // argv as well. An hour was wasted on this.
            execl(psearch2aslave_exe, psearch2aslave_exe, target_color_string,
                  input_files_names[i], index_str, length_str, null);

            printf("Slave process failed. Errno: %d\n", errno);
            exit(-1);
        }
    }

    i32 wstatus = 0;
    while (wait(&wstatus) > 0)
        ;

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);
    munmap(results, input_files_count * sizeof(color_parse_result));
    shm_unlink(psearch2a_shm_name);
    free(input_files_names);
}
