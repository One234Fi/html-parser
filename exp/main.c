#define FICKIT_IMPL
#include "fickit.h"
#include <unistd.h>
#include <stdio.h>

char * dir_string(string url, arena * perm, arena scratch) {
    strings temp = s_split(url, '/', &scratch);
    temp.len -= 4;
    temp.data++;
    temp.data++;
    size len = 0;
    for (size i = 0; i < temp.len; i++) {
        len += temp.data[i].len;
        len++;
    }
    return s_clone_c((string) {
        temp.data[0].data,
        len,
    }, perm);
}

string file_name(string url, arena * perm, arena scratch) {
    strings temp = s_split(url, '/', &scratch);
    return s_clone(temp.data[temp.len - 2], perm);
}



int main(int argc, char ** argv) {
    command c = {0};
    arena a = arena_init(1024 * 1024);
    arena loop = arena_init(1024 * 1024);
    string dat = read_file(argv[1], &a);
    strings lines = s_split(dat, '\n', &a);
    for (size i = 0; i < lines.len; i++) {
        string line = lines.data[i];
        fprintf(stderr, "%.*s\n", (int) line.len, line.data);
        char * dir = dir_string(line, &loop, a);
        fprintf(stderr, "dir: %s\n", dir);
        string file = file_name(line, &loop, a);
        file = s_cat(String(dir), file, &loop);
        char * file_c = s_clone_c(file, &loop);
        fprintf(stderr, "file: %.*s\n", (int) file.len, file.data);

        if (strlen(dir) > 0) {
            *push(&c, &loop) = "/bin/mkdir";
            *push(&c, &loop) = "-p";
            *push(&c, &loop) = dir;
            *push(&c, &loop) = NULL;
            print_command(c);
            c.len = 0;
        //    if (!run(&c, NULL, NULL)) {
        //        fprintf(stderr, "Failed to mkdir\n");
        //    };
        }

        if (file.len > 0 && strlen(file_c) > 0) {
            *push(&c, &loop) = "/bin/curl";
            *push(&c, &loop) = s_clone_c(line, &loop);
            *push(&c, &loop) = NULL;

            //*push(&c, &a) = file_c;
            //FILE * f = fopen(file_c, "wb");

            print_command(c);
            c.len = 0;
        //    if (!run2(&c, fileno(f))) {
        //        fprintf(stderr, "Failed to curl\n");
        //    };

            //fclose(f);

            //sleep(3);
        }

        loop.pos = loop.beg;
    }
    return 0;
}
