//bob can build it
#include <stdlib.h>
#define FICKIT_IMPL
#include "fickit.h"

#include <sys/types.h>
#include <dirent.h>

#include <unistd.h>
#include <sys/wait.h>

#define for_each(list, handle) for (typeof(*list) (* handle) = list; *handle != NULL; handle++)

typedef struct command {
    char ** data;
    size len;
    size cap;
} command;

int run(command * c) {
    int pid = fork();
    if (pid == 0) return execv(c->data[0], c->data);

    c->len = 0;
    return waitpid(pid, NULL, 0) == -1 ? 0 : 1;
}

void print_command(command c) {
    for (size i = 0; i < c.len - 1; i++) {
        printf("%s ", c.data[i]);
    }
    printf("\n");
}

//FIXME: hacky thing for POC
int ends_with(const char * str, const char * suf) {
    int a = strlen(str);
    int b = strlen(suf);

    for (; b > 0; b--, a--) {
        if (str[a] != suf[b]) {
            return 0;
        }
    }
    return 1;
}

char * target_of(const char * str, arena * a) {
    int len = strlen(str) + 1;
    char * s = new(a, char, len);
    memcpy(s, str, len);
    s[len - 2] = 'o';
    s[len - 1] = '\0';
    return s;
}

int src_file_cmp(const char * s) {
    return ends_with(s, ".c") 
        && (strcmp("bob.c", s) != 0) 
        && !ends_with(s, "_tst.c");
}

char ** get_files(char ** dirs, int (*compare)(const char *), arena * ptrs, arena * strs) {
    struct {
        char ** data;
        size len;
        size cap;
    } src_files = {0};
    for (char ** iter = dirs; *iter != NULL; iter++) {
        const char * str = *iter;
        struct dirent * entry;
        DIR * d = opendir(str);
        if (d) {
            while ((entry = readdir(d)) != NULL) {
                if (entry->d_type == DT_REG && compare(entry->d_name)) {
                    int name_len = strlen(str) + strlen(entry->d_name) + 1;
                    char * buf = new(strs, char, name_len);
                    memset(buf, 0, name_len);
                    strcat(buf, str);
                    strcat(buf, entry->d_name);
                    *push(&src_files, ptrs) = buf;
                }
            }
            if (closedir(d) != 0) {
                perror("closedir");
                exit(1);
            }
        }
    }
    *push(&src_files, ptrs) = NULL;
    return src_files.data;
}

int main(int argc, char * argv[]) {
    arena a = arena_init(4096 * 16);
    arena b = arena_init(4096 * 16);
    char * cc = "/bin/gcc";
    char * cc_flags[] = {
        "-std=c23",
        "-g3",
        "-Wall",
        "-Wextra",
		"-Wno-unused-parameter",
        "-Werror=return-type",
		"-fsanitize=undefined",
        "-fsanitize-recover=undefined",
        NULL
    };
    char * src_dirs[] = {
        "./",
        "./lexer/",
        NULL
    };
    char * inc_flags[] = {
        "-I./",
        "-I./lexer",
        NULL
    };
    char * linker_flags[] = {
        "-fsanitize=undefined",
        "-fsanitize-recover=undefined",
        NULL
    };

    char ** src_files = get_files(src_dirs, src_file_cmp, &a, &b);

    command c = {0};

    for_each (src_files, src_iter) {
        *push(&c, &a) = cc;

        for_each (cc_flags, iter) {
            *push(&c, &a) = *iter;
        }
        *push(&c, &a) = "-c";
        *push(&c, &a) = *src_iter;
        *push(&c, &a) = "-o";
        *push(&c, &a) = target_of(*src_iter, &b);
        for (char ** iter = inc_flags; *iter != NULL; iter++) {
            *push(&c, &a) = *iter;
        }

        print_command(c);
        int res = run(&c);
        if (res == -1) {
            printf("Failed to build\n");
        }
    }

    *push(&c, &a) = cc;
    for_each (cc_flags, iter) {
        *push(&c, &a) = *iter;
    }
    *push(&c, &a) = "-o";
    *push(&c, &a) = "main.out";
    for_each (linker_flags, iter) {
        *push(&c, &a) = *iter;
    }
    for_each (src_files, src_iter) {
        //ok to mutate now since compiling is done
        printf("%s\n", *src_iter);
        *push(&c, &a) = target_of(*src_iter, &b);
    }

    print_command(c);
    int res = run(&c);
    if (res == -1) {
        printf("Failed to build\n");
    }
}
