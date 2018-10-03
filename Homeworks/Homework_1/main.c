#include <stdio.h>
#include "file_system.h"

int main() {
    file_system* fs = file_system_create_file_system();
    file_system_touch_folder(fs, "hello");
    file_system_touch_file(fs, "world");
    file_system_cd(fs, "hello");
    file_system_touch_file(fs, "foo");
    file_system_cdup(fs);
    file_system_lsrecursive(fs);
}
