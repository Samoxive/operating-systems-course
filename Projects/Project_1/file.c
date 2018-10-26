#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "c_string.h"

char* read_file_to_string(char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        return null;
    }

    i32 fd;
    if ((fd = fileno(file)) == -1) {
        fclose(file);
        return null;
    }
    struct stat file_stats = {0};
    if (fstat(fd, &file_stats) == -1) {
        fclose(file);
        return null;
    }
    i64 size = file_stats.st_size;
    char* buf = malloc(size + 1);
    buf[size] = '\0';  // fread doesn't always null terminate as it claims to
    if (!fread(buf, sizeof(char), size, file)) {
        fclose(file);
        return null;
    }

    fclose(file);
    return buf;
}

bool write_struct_to_file(char* file_name, void* data, i32 size) {
    FILE* file = fopen(file_name, "wb");
    if (file == null) {
        return true;
    }

    fwrite(data, size, 1, file);
    fclose(file);
    return false;
}

bool read_struct_from_file(char* file_name, void* data, i32 size) {
    FILE* file = fopen(file_name, "rb");
    if (file == null) {
        return true;
    }

    fread(data, size, 1, file);
    fclose(file);
    return false;
}