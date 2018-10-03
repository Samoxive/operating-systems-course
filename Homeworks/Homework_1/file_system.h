#pragma once
#include <stdbool.h>
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
    file_system_entity_type entity_type;
    struct file_system_entity* next;
} file_system_entity;

typedef struct file_system_folder {
    struct file_system_entity entity;
    i32 children_entities_count;
    file_system_entity* child;
    struct file_system_folder* parent;
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

void file_system_panic(char* message, file_system* fs);
file_system_entity file_system_create_entity(char* name,
                                             file_system_entity_type type);
file_system_folder* file_system_create_folder(char* name);
file_system_file* file_system_create_file(char* name);
file_system* file_system_create_file_system();
void file_system_cleanup_file(file_system_file* file);
void file_system_cleanup_folder(file_system* fs, file_system_folder* folder);
void file_system_cleanup_entity(file_system* fs, file_system_entity* entity);
file_system_entity* file_system_get_entity_by_name(file_system* fs, char* name);
bool file_system_name_exists_in_folder(file_system* fs, char* name);
void file_system_add_entity_to_folder(file_system* fs,
                                      file_system_entity* entity);
file_system_entity* file_system_remove_entity_from_folder_fiber(
    file_system* fs,
    file_system_entity* fiber_head,
    char* entity_name,
    bool* removed);
void file_system_remove_entity_from_folder(file_system* fs, char* entity_name);
void file_system_touch_file(file_system* fs, char* file_name);
void file_system_touch_folder(file_system* fs, char* folder_name);
void file_system_print_file(file_system_file* file);
void file_system_print_folder(file_system_folder* folder);
void file_system_print_entity(file_system* fs, file_system_entity* entity);
void file_system_ls(file_system* fs);
void file_system_lsrecursive_impl(file_system* fs,
                                  file_system_folder* folder,
                                  int level);
void file_system_lsrecursive(file_system* fs);
void file_system_pwd(file_system* fs);
void file_system_cd(file_system* fs, char* target_name);
void file_system_cdup(file_system* fs);
void file_system_modify_file(file_system* fs, char* name, char* new_content);
void file_system_rm(file_system* fs, char* name);
void file_system_mov(file_system* fs,
                     char* target_name,
                     char* target_folder_name);
