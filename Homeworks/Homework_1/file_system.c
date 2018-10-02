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
    file_system_entity entity = {.name = name,
                                 .creation_date = t,
                                 .modify_date = t,
                                 .entity_type = type,
                                 .next = null};

    return entity;
}

file_system_folder* file_system_create_folder(char* name) {
    file_system_folder* folder = malloc(sizeof(file_system_folder));
    folder->entity = file_system_create_entity(name, FOLDER_TYPE);
    folder->children_entities_count = 0;
    folder->child = null;
    folder->parent = null;

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

file_system* file_system_create_file_system() {
    file_system* fs = malloc(sizeof(file_system));
    fs->root = file_system_create_folder("/");
    fs->cursor = fs->root;
    return fs;
}

void file_system_cleanup_entity(file_system* fs, file_system_entity* entity);

void file_system_cleanup_file(file_system_file* file) {
    free(file->entity.name);
    free(file->content);
    free(file);
}

void file_system_cleanup_folder(file_system* fs, file_system_folder* folder) {
    free(folder->entity.name);
    if (folder->child != null) {
        file_system_cleanup_entity(fs, folder->child);
    }
    free(folder);
}

void file_system_cleanup_entity(file_system* fs, file_system_entity* entity) {
    if (entity->entity_type == FILE_TYPE) {
        file_system_cleanup_file((file_system_file*)entity);
    } else if (entity->entity_type == FOLDER_TYPE) {
        file_system_cleanup_folder(fs, (file_system_folder*)entity);
    } else {
        file_system_panic("inconsistent state of file system entity seen!", fs);
    }
}

file_system_entity* file_system_get_entity_by_name(file_system* fs,
                                                   char* name) {
    file_system_folder* folder = fs->cursor;
    file_system_entity* ptr = folder->child;
    while (ptr != null) {
        if (strcmp(name, ptr->name) == 0) {
            return ptr;
        }

        ptr = ptr->next;
    }

    return null;
}

bool file_system_name_exists_in_folder(file_system* fs, char* name) {
    return file_system_get_entity_by_name(fs, name) != null;
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
    file_system* fs,
    file_system_entity* fiber_head,
    char* entity_name,
    bool* removed) {
    if (fiber_head == null) {
        return null;
    }

    if (strcmp(entity_name, fiber_head->name) == 0) {
        file_system_entity* next_entity = fiber_head->next;
        fiber_head->next = null;
        file_system_cleanup_entity(fs, fiber_head);
        *removed = true;
        return next_entity;
    }

    fiber_head->next = file_system_remove_entity_from_folder_fiber(
        fs, fiber_head->next, entity_name, removed);
    return fiber_head;
}

void file_system_remove_entity_from_folder(file_system* fs, char* entity_name) {
    bool removed = false;
    file_system_folder* folder = fs->cursor;
    folder->child = file_system_remove_entity_from_folder_fiber(
        fs, folder->child, entity_name, &removed);
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
    folder->parent = fs->cursor;
    file_system_add_entity_to_folder(fs, (file_system_entity*)folder);
}

void file_system_print_file(file_system_file* file) {
    struct tm creation_date = *localtime(&file->entity.creation_date);
    struct tm modification_date = *localtime(&file->entity.modify_date);
    char creation_date_buf[64];
    char modification_date_buf[64];
    strftime(creation_date_buf, 64, "%d %b %H:%M", &creation_date);
    strftime(modification_date_buf, 64, "%d %b %H:%M", &modification_date);
    printf("%dB %s %s %s\n", file->size, creation_date_buf,
           modification_date_buf, file->entity.name);
}

void file_system_print_folder(file_system_folder* folder) {
    struct tm creation_date = *localtime(&folder->entity.creation_date);
    struct tm modification_date = *localtime(&folder->entity.modify_date);
    char creation_date_buf[64];
    char modification_date_buf[64];
    strftime(creation_date_buf, 64, "%d %b %H:%M", &creation_date);
    strftime(modification_date_buf, 64, "%d %b %H:%M", &modification_date);
    printf("%d item(s) %s %s %s\n", folder->children_entities_count,
           creation_date_buf, modification_date_buf, folder->entity.name);
}

void file_system_print_entity(file_system* fs, file_system_entity* entity) {
    if (entity->entity_type == FILE_TYPE) {
        file_system_file* file = (file_system_file*)entity;
    } else if (entity->entity_type == FOLDER_TYPE) {
        file_system_folder* folder = (file_system_folder*)entity;
    } else {
        file_system_panic("inconsistent state of file system entity seen!", fs);
    }
}

void file_system_ls(file_system* fs) {
    file_system_entity* ptr = fs->cursor->child;
    while (ptr != null) {
        file_system_print_entity(fs, ptr);
        ptr = ptr->next;
    }
}

void file_system_lsrecursive_impl(file_system* fs,
                                  file_system_folder* folder,
                                  int level) {
    file_system_entity* ptr = folder->child;
    while (ptr != null) {
        for (int i = 0; i < level; i++) {
            printf("-");
        }
        file_system_print_entity(fs, ptr);
        if (ptr->entity_type == FOLDER_TYPE) {
            file_system_lsrecursive_impl(fs, (file_system_folder*)ptr,
                                         level + 1);
        }
    }
}

void file_system_lsrecursive(file_system* fs) {
    file_system_lsrecursive_impl(fs, fs->cursor, 0);
}

void file_system_pwd(file_system* fs) {
    file_system_folder* ptr = fs->cursor;
    char* folder_names[64];  // pls no break with 64 deep folders
    int folders = 0;
    do {
        folder_names[folders] = ptr->entity.name;
        folders++;
        ptr = ptr->parent;
    } while (ptr->parent != null);

    for (int i = (folders - 1); i > -1; i--) {
        printf("%s/", folder_names[i]);
    }
    printf("\n");
}
