#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common_include.h"

const char* MQ_NAME = "/results_mq";
const char* SLAVE_EXE = "./psearch2cslave";

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

    struct mq_attr message_queue_attr = {
        .mq_maxmsg = 1, .mq_msgsize = sizeof(pid_color_parse_result)};
    mqd_t message_queue = mq_open(MQ_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR,
                                  &message_queue_attr);
    if (message_queue == -1) {
        printf("Failed to create message queue.\n");
        char s[100];
        perror(s);
        exit(-1);
    }

    pid_t* slave_pids = malloc(input_files_count * sizeof(pid_t));
    for (i32 i = 0; i < input_files_count; i++) {
        pid_t f = fork();
        if (f == 0) {
            char index_str[12];
            char length_str[12];
            sprintf(index_str, "%d", i);
            sprintf(length_str, "%d", input_files_count);
            execl(SLAVE_EXE, SLAVE_EXE, target_color_string,
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
        if (mq_receive(message_queue, (char*)&result,
                       sizeof(pid_color_parse_result), null) == -1) {
            printf("Could not receive from message queue.\n");
            exit(-1);
        }
        i32 index =
            get_index_from_pid_array(slave_pids, input_files_count, result.pid);
        results[index] = result.result;
    }

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);

    mq_close(message_queue);
    mq_unlink(MQ_NAME);
    free(results);
    free(input_files_names);
    free(slave_pids);
    return 0;
}