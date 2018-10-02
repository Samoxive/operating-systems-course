#pragma once
#include <time.h>
#include "typedefs.h"

typedef enum file_system_entity_type {
    FOLDER_TYPE,
    FILE_TYPE
} file_system_entity_type;

typedef struct file_system_entity {
    char* name;
    time_t creation_date;
    time_t modify_date;
    struct file_system_entity* next;
} file_system_entity;

typedef struct file_system_folder {
    struct file_system_entity entity;
    i32 children_entities_count;
    file_system_entity* child;
} file_system_folder;

typedef struct file_system_file {
    struct file_system_entity entity;
    i32 size;
    char* content;
} file_system_file;

typedef struct file_system {
    struct file_system_folder* root;
    struct file_system_folder* cursor;

} file_system;
