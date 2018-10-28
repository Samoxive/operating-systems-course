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

const char* message_queue_name = "/psearch_mq";

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
    mqd_t message_queue = mq_open(message_queue_name, O_RDWR);
    if (message_queue == -1) {
        printf("Failed to open message queue.\n");
        exit(-1);
    }

    pid_color_parse_result anon_result = {.pid = getpid(), .result = result};
    if (mq_send(message_queue, (char*)&anon_result,
                sizeof(pid_color_parse_result), 0) == -1) {
        printf("Failed to send message.\n");
        exit(-1);
    }

    mq_close(message_queue);
    free(file_content);
    return 0;
}
