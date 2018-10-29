#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
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

typedef struct {
    i32 index;
    color_parse_result result;
} thread_output;

typedef struct thread_output_list_node {
    thread_output* output;
    struct thread_output_list_node* next;
} thread_output_list_node;

typedef struct {
    thread_output_list_node* list;
    char* input_file_name;
    i32 index;
    sem_t* lock;
} thread_input;

void* thread_it(void* data) {
    thread_input* input = data;
    char* file_content = read_file_to_string(input->input_file_name);
    color_parse_result result = parse_string_into_colors(file_content);

    if (sem_wait(input->lock) == -1) {
        printf("Failed to lock semaphore.\n");
        exit(-1);
    }

    thread_output* output = malloc(sizeof(thread_output));
    output->index = input->index;
    output->result = result;

    if (input->list->output == null) {
        input->list->output = output;
    } else {
        thread_output_list_node* new_node =
            malloc(sizeof(thread_output_list_node));
        new_node->next = null;
        new_node->output = output;

        thread_output_list_node* ptr = input->list;
        while (ptr->next != null) {
            ptr = ptr->next;
        }
        ptr->next = new_node;
    }

    if (sem_post(input->lock) == -1) {
        printf("Failed to unlock semaphore.\n");
        exit(-1);
    }

    free(file_content);
    return null;
}

int main(int argc, char** argv) {
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
    pthread_t* thread_handles = malloc(input_files_count * sizeof(pthread_t));
    thread_input* inputs = malloc(input_files_count * sizeof(thread_input));
    thread_output_list_node* output_list_head =
        malloc(sizeof(thread_output_list_node));
    output_list_head->output = null;
    output_list_head->next = null;

    sem_t lock;
    sem_init(&lock, 0, 1);

    for (i32 i = 0; i < input_files_count; i++) {
        thread_input input = {.list = output_list_head,
                              .input_file_name = input_files_names[i],
                              .index = i,
                              .lock = &lock};
        inputs[i] = input;
    }

    for (i32 i = 0; i < input_files_count; i++) {
        if (pthread_create(&(thread_handles[i]), null, thread_it,
                           &(inputs[i]))) {
            printf("Failed creating a thread.\n");
            exit(-1);
        }
    }

    for (i32 i = 0; i < input_files_count; i++) {
        if (pthread_join(thread_handles[i], null)) {
            printf("Failed joining a thread.\n");
            exit(-1);
        }
    }

    color_parse_result* results =
        malloc(input_files_count * sizeof(color_parse_result));

    if (output_list_head->output == null) {
        printf("No result was found, somehow...\n");
        exit(-1);
    }

    thread_output_list_node* ptr = output_list_head;
    do {
        results[ptr->output->index] = ptr->output->result;
        ptr = ptr->next;
    } while (ptr != null);

    write_final_output_to_file(target_color, input_files_count, results,
                               input_files_names, output_file_name);

    sem_destroy(&lock);
    free(input_files_names);
    do {
        thread_output_list_node* next_head = output_list_head->next;
        if (output_list_head->output != null) {
            free(output_list_head->output);
        }
        free(output_list_head);
        output_list_head = next_head;
    } while (output_list_head != null);
    free(inputs);
    free(thread_handles);
    free(results);
    return 0;
}