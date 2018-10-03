#include <stdio.h>
#include <string.h>
#include "file_system.h"

void test() {
    file_system* fs = file_system_create_file_system();
    file_system_touch_folder(fs, strdup("hello"));
    file_system_touch_file(fs, strdup("world"));
    file_system_cd(fs, "hello");
    file_system_touch_file(fs, strdup("foo"));
    file_system_touch_file(fs, strdup("foo1"));
    file_system_touch_file(fs, strdup("foo2"));
    file_system_touch_file(fs, strdup("foo3"));
    file_system_touch_file(fs, strdup("foo4"));
    file_system_ls(fs);
    file_system_pwd(fs);
    file_system_modify_file(fs, "foo", strdup("hello i am smart"));
    file_system_rm(fs, "foo3");
    file_system_cdup(fs);
    file_system_mov(fs, "world", "hello");
    file_system_lsrecursive(fs);
    file_system_cd(fs, "hello");
    file_system_touch_folder(fs, strdup("nested_folder"));
    file_system_mov(fs, "world", "nested_folder");
    file_system_cdup(fs);
    file_system_lsrecursive(fs);
    file_system_cleanup_file_system(fs);
}

int main() {
    test();
    return 0;
}
