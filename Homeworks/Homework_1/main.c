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
    // test();
    file_system* fs = file_system_create_file_system();
    char input_buf[1024] = {0};
    printf("Type `exit` to quit.\n");
    while (true) {
        printf("> ");
        scanf("%s", input_buf);
        if (strcmp("ls", input_buf) == 0) {
            file_system_ls(fs);
        } else if (strcmp("lsrecursive", input_buf) == 0) {
            file_system_lsrecursive(fs);
        } else if (strcmp("mkdir", input_buf) == 0) {
            scanf("%s", input_buf);
            file_system_touch_folder(fs, strdup(input_buf));
        } else if (strcmp("rm", input_buf) == 0) {
            scanf("%s", input_buf);
            file_system_rm(fs, input_buf);
        } else if (strcmp("mov", input_buf) == 0) {
            scanf("%s", input_buf);
            char target_folder_input_buf[1024] = {0};
            scanf("%s", target_folder_input_buf);
            file_system_mov(fs, input_buf, target_folder_input_buf);
        } else if (strcmp("pwd", input_buf) == 0) {
            file_system_pwd(fs);
        } else if (strcmp("cd", input_buf) == 0) {
            scanf("%s", input_buf);
            file_system_cd(fs, input_buf);
        } else if (strcmp("cdup", input_buf) == 0) {
            file_system_cdup(fs);
        } else if (strcmp("edit", input_buf) == 0) {
            printf("Please enter some text to overwrite the file content:\n\n");
            scanf("%s ", input_buf);
            char new_content_buf[1024] = {0};
            fgets(new_content_buf, sizeof(new_content_buf), stdin);
            new_content_buf[strlen(new_content_buf) - 1] = '\0';
            file_system_modify_file(fs, input_buf, strdup(new_content_buf));
        } else if (strcmp("touch", input_buf) == 0) {
            scanf("%s", input_buf);
            file_system_touch_file(fs, strdup(input_buf));
        } else if (strcmp("exit", input_buf) == 0) {
            break;
        } else {
            printf("Invalid operation! Try again.\n");
        }
    }
    file_system_cleanup_file_system(fs);
    return 0;
}
