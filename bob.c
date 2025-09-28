//bob can build it
// cc bob.c -o bob.out
// ./bob.out
#include <stdlib.h>
#include <string.h>
#define FICKIT_IMPL
#include "fickit.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define for_each(list, handle) for (typeof(*list) (* handle) = list; *handle != NULL; handle++)

typedef struct command {
    char ** data;
    size len;
    size cap;
} command;

typedef struct {
    int * data;
    size len;
    size cap;
} pids;

//TODO: limit fork count? fine for now, but could get bad if number of files increases a lot
int run(command * c, pids * p, arena * a) {
    int pid = fork();
    if (pid == 0) return execv(c->data[0], c->data);

    c->len = 0;
    if (p) {
        *push(p, a) = pid;
        return 1;
    }
    return waitpid(pid, NULL, 0) == -1 ? 0 : 1;
}

int await_all(pids * p) {
    while (p->len > 0) {
        int status = 0;
        int pid = wait(&status);
        p->len--;
        if (pid == -1) {
            return 0;
        }
        if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
            if (status) {
                return !status;
            }
        }
    }
    return 1;
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

char * object_of(const char * str, arena * a) {
    int len = strlen(str) + 1;
    char * s = new(a, char, len);
    memcpy(s, str, len);
    s[len - 2] = 'o';
    s[len - 1] = '\0';
    return s;
}

char * target_of(const char * str, arena * a) {
    int len = strlen(str) + 3;
    char * s = new(a, char, len);
    memcpy(s, str, len);
    s[len - 4] = 'o';
    s[len - 3] = 'u';
    s[len - 2] = 't';
    s[len - 1] = '\0';
    return s;
}

int src_file_cmp(const char * s) {
    return ends_with(s, ".c")
        && (strcmp("bob.c", s) != 0)
        && (strcmp("main.c", s) != 0)
        && !ends_with(s, "_tst.c");
}

int target_file_cmp(const char * s) {
    return (strcmp("main.c", s) == 0)
        || ends_with(s, "_tst.c");
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
                if (compare(entry->d_name)) {
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

int compare_age(char * file_a, char * file_b) {
    struct stat b_stat;
    int bfd = open(file_b, O_RDONLY);
    int res = fstat(bfd, &b_stat);
    if (res != 0) {
        perror("file_b fstat");
        close(bfd);
        return 1;
    }
    close(bfd);

    struct stat a_stat;
    int afd = open(file_a, O_RDONLY);
    res = fstat(afd, &a_stat);
    if (res != 0) {
        perror("file_a fstat");
        close(afd);
        return -1;
    }
    close(afd);

    return a_stat.st_mtime - b_stat.st_mtime;
}

void bob_bootstrap(char * bob_name, char * bob_cc, char ** bob_flags) {
    command c = {0};
    arena a = arena_init(1024);

    if (compare_age(bob_name, "./bob.c") < 0) {
        printf("Bootstrapping bob:\n");
        printf("--------------------------------------------------------------------------------\n");

        *push(&c, &a) = "/bin/mv";
        *push(&c, &a) = bob_name;
        *push(&c, &a) = "bob.old";
        *push(&c, &a) = NULL;
        print_command(c);
        if (!run(&c, NULL, NULL)) {
            printf("Failed to move bob\n");
            exit(1);
        }

        *push(&c, &a) = bob_cc;
        for_each (bob_flags, iter) {
            *push(&c, &a) = *iter;
        }
        *push(&c, &a) = "./bob.c";
        *push(&c, &a) = "-o";
        *push(&c, &a) = bob_name;
        *push(&c, &a) = NULL;
        print_command(c);
        if (!run(&c, NULL, NULL)) {
            *push(&c, &a) = "/bin/mv";
            *push(&c, &a) = "bob.old";
            *push(&c, &a) = bob_name;
            *push(&c, &a) = NULL;
            print_command(c);
            if (!run(&c, NULL, NULL)) {
                printf("Failed to move bob.old\n");
                exit(1);
            }

            printf("Failed to bootrap bob\n");
            exit(1);
        }
        printf("Successfully bootstrapped bob\n");
        printf("--------------------------------------------------------------------------------\n");

        *push(&c, &a) = bob_name;
        *push(&c, &a) = NULL;
        if (!run(&c, NULL, NULL)) {
            *push(&c, &a) = "/bin/mv";
            *push(&c, &a) = "bob.old";
            *push(&c, &a) = bob_name;
            *push(&c, &a) = NULL;
            print_command(c);
            if (!run(&c, NULL, NULL)) {
                printf("Failed to move bob.old\n");
                exit(1);
            }

            printf("Failed to run bob.out after rebuild\n");
            exit(1);
        }

        exit(0);
    }
}

int main(int argc, char * argv[]) {
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
    bob_bootstrap(argv[0], cc, cc_flags);

    arena a = arena_init(4096 * 16);
    arena b = arena_init(4096 * 16);
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
    char ** target_files = get_files(src_dirs, target_file_cmp, &a, &b);
    struct {
        char ** data;
        size len;
        size cap;
    } built_files = {0};

    command c = {0};
    pids p = {0};
    arena pid_buf = arena_init(sizeof(int) * 256);

    for_each (src_files, src_iter) {
        char * target_obj = object_of(*src_iter, &b);
        if (compare_age(*src_iter, target_obj) > 0) {
            *push(&c, &a) = cc;

            for_each (cc_flags, iter) {
                *push(&c, &a) = *iter;
            }
            for (char ** iter = inc_flags; *iter != NULL; iter++) {
                *push(&c, &a) = *iter;
            }
            *push(&c, &a) = "-c";
            *push(&c, &a) = *src_iter;
            *push(&c, &a) = "-o";
            *push(&c, &a) = object_of(*src_iter, &b);
            *push(&c, &a) = NULL;

            print_command(c);
            int res = run(&c, &p, &pid_buf);
            if (res == -1) {
                printf("Failed to build\n");
            }
            *push(&built_files, &b) = target_obj;
        }
    }

    for_each (target_files, target) {
        char * target_obj = object_of(*target, &b);
        if (compare_age(*target, target_obj) > 0) {
            *push(&c, &a) = cc;

            for_each (cc_flags, iter) {
                *push(&c, &a) = *iter;
            }
            for (char ** iter = inc_flags; *iter != NULL; iter++) {
                *push(&c, &a) = *iter;
            }
            *push(&c, &a) = "-c";
            *push(&c, &a) = *target;
            *push(&c, &a) = "-o";
            *push(&c, &a) = object_of(*target, &b);
            *push(&c, &a) = NULL;

            print_command(c);
            int res = run(&c, &p, &pid_buf);
            if (res == -1) {
                printf("Failed to build\n");
            }
            *push(&built_files, &b) = target_obj;
        }
    }
    if (!await_all(&p)) {
        printf("Failed to build\n");
        exit(1);
    }

    for_each(target_files, target) {
        char * target_obj = target_of(*target, &b);
        if (built_files.len > 0) {
            *push(&c, &a) = cc;
            for_each (cc_flags, iter) {
                *push(&c, &a) = *iter;
            }
            *push(&c, &a) = "-o";
            *push(&c, &a) = target_of(*target, &b);
            for_each (linker_flags, iter) {
                *push(&c, &a) = *iter;
            }
            for_each (src_files, src_iter) {
                //ok to mutate now since compiling is done
                *push(&c, &a) = object_of(*src_iter, &b);
            }
            *push(&c, &a) = object_of(*target, &b);
            *push(&c, &a) = NULL;

            print_command(c);
            int res = run(&c, &p, &pid_buf);
            if (res == -1) {
                printf("Failed to build: %s\n", *target);
            }
            *push(&built_files, &b) = target_obj;
        }
    }
    if (!await_all(&p)) {
        printf("Failed to build\n");
        exit(1);
    }
    *push(&built_files, &b) = NULL;
    
    printf("--------------------------------------------------------------------------------\n");
    if (built_files.len == 1) {
        printf("No Files Changed\n");
    } else {
        printf("Successfully built:\n");
        for_each(built_files.data, file) {
            printf("\t%s\n", *file);
        }
    }
    printf("--------------------------------------------------------------------------------\n");
}
