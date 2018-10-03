#include <stdio.h>
#include "file_system.h"

void test() {
    file_system* fs = file_system_create_file_system();
    file_system_touch_folder(fs, "hello");
    file_system_touch_file(fs, "world");
    file_system_cd(fs, "hello");
    file_system_touch_file(fs, "foo");
    file_system_touch_file(fs, "foo1");
    file_system_touch_file(fs, "foo2");
    file_system_touch_file(fs, "foo3");
    file_system_touch_file(fs, "foo4");
    file_system_cdup(fs);
    file_system_lsrecursive(fs);
    file_system_cleanup_file_system(fs);
}

int main() {
    test();
    return 0;
}
