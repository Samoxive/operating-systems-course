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

const char* psearch2dslave_exe = "./psearch2dslave";
static char* psearch2d_fifo_name = "./psearch2d.tmp";

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
    if (mkfifo(psearch2d_fifo_name, S_IRUSR | S_IWUSR) == -1) {
        printf("Failed to open fifo pipe.\n");
        exit(-1);
    }
    i32 fifo_fd = open(psearch2d_fifo_name, O_RDWR);

    pid_t* slave_pids = malloc(input_files_count * sizeof(pid_t));
    for (i32 i = 0; i < input_files_count; i++) {
        pid_t f = fork();
        if (f == 0) {
            execl(psearch2dslave_exe, psearch2dslave_exe, target_color_string,
                  input_files_names[i], null);

            printf("Slave process failed. Errno: %d\n", errno);
            exit(-1);
        } else {
            slave_pids[i] = f;
        }
    }

    color_parse_result* results =
        malloc(input_files_count * sizeof(color_parse_result));
    for (i32 i = 0; i < input_files_count; i++) {
        pid_color_parse_result result = {0};
        if (read(fifo_fd, &result, sizeof(pid_color_parse_result)) == -1) {
            printf("Could not read from FIFO file.\n");
            exit(-1);
        }
        i32 index =
            get_index_from_pid_array(slave_pids, input_files_count, result.pid);
        results[index] = result.result;
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);

    close(fifo_fd);
    remove(psearch2d_fifo_name);
    free(results);
    free(input_files_names);
    free(slave_pids);
    return 0;
}