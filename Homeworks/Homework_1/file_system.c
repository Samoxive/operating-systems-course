#include "file_system.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void file_system_panic(char* message, file_system* fs) {
    // TODO(sam): fs cleanup
    printf("FATAL_ERROR: %s\n", message);
    exit(1);
}

file_system_entity file_system_create_entity(char* name,
                                             file_system_entity_type type) {
    time_t t = time(null);
    file_system_entity entity = {
        .name = name, .creation_date = t, .modify_date = t, .next = null};

    return entity;
}

file_system_folder* file_system_create_folder(char* name) {
    file_system_folder* folder = malloc(sizeof(file_system_folder));
    folder->entity = file_system_create_entity(name, FOLDER_TYPE);
    folder->children_entities_count = 0;
    folder->child = null;

    return folder;
}

file_system_file* file_system_create_file(char* name) {
    file_system_file* file = malloc(sizeof(file_system_file));
    file->entity = file_system_create_entity(name, FILE_TYPE);
    file->size = 0;
    char* content = malloc(sizeof(char));
    content[0] = '\0';
    file->content = content;

    return file;
}

void file_system_cleanup_entity(file_system_entity* entity) {
    // TODO(sam): implement
}

bool file_system_name_exists_in_folder(file_system* fs, char* name) {
    file_system_folder* folder = fs->cursor;
    file_system_entity* ptr = folder->child;
    while (ptr != null) {
        if (strcmp(name, ptr->name) == 0) {
            return true;
        }

        ptr = ptr->next;
    }

    return false;
}

void file_system_add_entity_to_folder(file_system* fs,
                                      file_system_entity* entity) {
    file_system_folder* folder = fs->cursor;
    file_system_entity* ptr = folder->child;
    if (ptr == null) {
        folder->child = entity;
    } else {
        while (ptr->next != null) {
            ptr = ptr->next;
        }

        ptr->next = entity;
    }

    folder->entity.modify_date = time(null);
    folder->children_entities_count++;
}

file_system_entity* file_system_remove_entity_from_folder_fiber(
    file_system_entity* fiber_head,
    char* entity_name,
    bool* removed) {
    if (fiber_head == null) {
        return null;
    }

    if (strcmp(entity_name, fiber_head->name) == 0) {
        file_system_entity* next_entity = fiber_head->next;
        fiber_head->next = null;
        file_system_cleanup_entity(fiber_head);
        *removed = true;
        return next_entity;
    }

    fiber_head->next = file_system_remove_entity_from_folder_fiber(
        fiber_head->next, entity_name, removed);
    return fiber_head;
}

void file_system_remove_entity_from_folder(file_system* fs, char* entity_name) {
    bool removed = false;
    file_system_folder* folder = fs->cursor;
    folder->child = file_system_remove_entity_from_folder_fiber(
        folder->child, entity_name, &removed);
    if (removed) {
        file_system_panic(
            "could not find an entity with specified name in the folder to "
            "remove!",
            fs);
    } else {
        folder->entity.modify_date = time(null);
        folder->children_entities_count--;
    }
}

void file_system_touch_file(file_system* fs, char* file_name) {
    bool exists = file_system_name_exists_in_folder(fs, file_name);
    if (exists) {
        file_system_panic(
            "a folder or file with the given name already exists!", fs);
    }

    file_system_file* file = file_system_create_file(file_name);
    file_system_add_entity_to_folder(fs, (file_system_entity*)file);
}

void file_system_touch_folder(file_system* fs, char* folder_name) {
    bool exists = file_system_name_exists_in_folder(fs, folder_name);
    if (exists) {
        file_system_panic(
            "a folder or file with the given name already exists!", fs);
    }

    file_system_folder* folder = file_system_create_folder(folder_name);
    file_system_add_entity_to_folder(fs, (file_system_entity*)folder);
}
