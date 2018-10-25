#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "c_string.h"

char* read_file_to_string(char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        return null;
    }

    int fd;
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
    buf[size] = '\0'; // fread doesn't always null terminate as it claims to
    if (!fread(buf, sizeof(char), size, file)) {
        fclose(file);
        return null;
    }

    fclose(file);
    return buf;
}