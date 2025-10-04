/*
 * fickit.h
 * 
 * various utilities
 */

#ifndef FICKIT_H
#define FICKIT_H

//TODO: I would like to not depend on stdlib at some point
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int64_t size;
typedef uint64_t usize;

#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define LOG_INFO(message) fprintf(stderr, BLUE"%s, %s, %d:"COLOR_RESET" \"%s\"\n", __FILE__, __func__, __LINE__, message)
#define LOG_ERROR(message) fprintf(stderr, BLUE"%s, %s, %d:"RED" \"%s\"\n"COLOR_RESET, __FILE__, __func__, __LINE__, message)
#define LOG_WARN(message) fprintf(stderr, BLUE"%s, %s, %d:"YELLOW" \"%s\"\n"COLOR_RESET, __FILE__, __func__, __LINE__, message)
#define ASSERT(expr, message, return_val) if (!(expr)) { LOG_ERROR(message); return return_val; } 

#define str(x) #x
#define xstr(x) str(x)
#define die(message) LOG_ERROR(message); exit(EXIT_FAILURE)

#define DEFINE_ENUM(ENUM) ENUM,
#define MAKE_ENUM_STRINGS(STRING) #STRING,

//Arena
#define new(...)                    newx(__VA_ARGS__,new4,new3,new2)(__VA_ARGS__)
#define newx(a, b, c, d, e, ...)    e
#define new2(a, t)                  (t *)alloc(a, sizeof(t), _Alignof(t), 1)
#define new3(a, t, n)               (t *)alloc(a, sizeof(t), _Alignof(t), n)

typedef struct {
    char * beg;
    char * pos;
    char * end;
    char * debug_name;
} arena;

arena arena_init(size cap);
arena arena_wrap(size cap, void * mem);
arena arena_slice(arena * a, size cap);
void * alloc(arena * a, size stride, size align, size count);


//Vectors
#define push(s, arena)  ((s)->len >= (s)->cap ? _grow(s, sizeof(*(s)->data), arena), (s)->data + (s)->len++ : (s)->data + (s)->len++) 
#define push_front(s, arena) (_shift_right_impl(s, 1, sizeof(*(s)->data), arena), (s)->data)
#define pop_front(s) (_shift_left_impl(s, 1, sizeof(*(s)->data)))

void _grow(void * slice, size stride, arena * a);
void _shift_right_impl(void * slice, size index, size stride, arena * a);
void _shift_left_impl(void * slice, size index, size stride);


typedef struct string {
    char * data;
    size len;
} string;

typedef struct {
    string * data;
    size len;
    size cap;
} strings;

typedef struct {
    char * data;
    size len;
    size cap;
} string_buffer;

#define String(s) (string){(char*)s, strlen(s)}
bool s_equal(string a, string b);
#define s_equal_c(s, cstr) s_equal(s, String(cstr))
bool s_equal_ignore_case(string a, string b);
string s_clone(string s, arena * a);
char * s_clone_c(string s, arena * a);
string s_cat(string a, string b, arena * perm);
string s_reverse(string s);
strings s_split(string s, char delim, arena * a);


//bob
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

int run(command * c, pids * p, arena * a);
int run2(command * c, int fdout);
int await_all(pids * p);
void print_command(command c);
int compare_age(char * file_a, char * file_b);
void bob_bootstrap(char * bob_name, char * bob_cc, char ** bob_flags);

//files
string read_file(char * filename, arena * a);

#endif



#ifdef FICKIT_IMPL
#undef FICKIT_IMPL

//TODO: I would like to not depend on stdlib at some point
#include <ctype.h>
#include <strings.h>

//Arena

arena arena_init(size cap) {
    arena a = {0};
    a.beg = malloc(cap);
    a.pos = a.beg;
    a.end = a.beg ? a.beg + cap : 0;
    return a;
}

arena arena_wrap(size cap, void * mem) {
    arena a = {0};
    a.beg = mem;
    a.pos = a.beg;
    a.end = a.beg + cap;
    return a;
}

arena arena_slice(arena * a, size cap) {
    char * backing = new(a, char, cap);
    return arena_wrap(cap, backing);
}

void * alloc(arena * a, size stride, size align, size count) {
    assert(stride != 0);
    size padding = -(usize)a->pos & (align - 1);
    size available = a->end - a->pos - padding;
    if (available < 0 || count > available / stride) {
        if (a->debug_name != NULL) {
            fprintf(stderr, "%s", a->debug_name);
        } 
        die("Arena OOM");
    }
    void * p = a->pos + padding;
    a->pos += padding + count * stride;
    return memset(p, 0, count * stride);
}



//Vectors

void _grow(void * slice, size stride, arena * a) {
    struct {
        void * data;
        size len;
        size cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));
    assert(temp.len >= 0);
    assert(temp.cap >= 0);
    assert(temp.len <= temp.cap);

    size align = 16;
    if (!temp.data) {
        temp.cap = 1;
        temp.data = alloc(a, stride * 2, align, temp.cap);
    }
    else if (a->beg == temp.data + stride * temp.cap) {
        alloc(a, stride, 1, temp.cap); 
    }
    else {
        void * data = alloc(a, stride * 2, align, temp.cap);
        memcpy(data, temp.data, stride * temp.len);
        temp.data = data;
    }

    temp.cap *= 2;
    memcpy(slice, &temp, sizeof(temp));
}

void _shift_right_impl(void * slice, size index, size stride, arena * a) {
    assert(slice != NULL);
    assert(index > 0);
    assert(stride > 0);
    struct {
        void * data;
        size len;
        size cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));

    if (index + temp.len >= temp.cap) {
        assert(a != NULL);
        _grow(&temp, stride, a);
    }

    memmove(temp.data + (index * stride), temp.data, temp.len * stride);
    memset(temp.data, 0, stride * index);
    temp.len += index;
    memcpy(slice, &temp, sizeof(temp));
}

void _shift_left_impl(void * slice, size index, size stride) {
    assert(slice != NULL);
    assert(index > 0);
    assert(stride > 0);
    struct {
        void * data;
        size len;
        size cap;
    } temp;
    memcpy(&temp, slice, sizeof(temp));

    memmove(temp.data, temp.data + (index * stride), (temp.len - index) * stride);
    temp.len -= index;
    memcpy(slice, &temp, sizeof(temp));
}



//Strings

bool s_equal(const string a, const string b) {
    return a.len == b.len && (strncmp(a.data, b.data, a.len) == 0);
}


bool s_equal_ignore_case(string a, string b) {
    if (a.len != b.len) {
        return false;
    }

    for (size i = 0; i < a.len; i++) {
        if (tolower(a.data[i]) != tolower(b.data[i])) {
            return false;
        }
    }

    return true;
}

string s_clone(string s, arena * a) {
    string n = s;
    n.data = new(a, char, s.len);
    if (n.len > 0) {
        memcpy(n.data, s.data, n.len);
    }
    return n;
}

char * s_clone_c(string s, arena * a) {
    char * n = new(a, char, s.len + 1);
    if (s.len > 0) {
        memcpy(n, s.data, s.len);
    }
    n[s.len] = '\0';
    return n;
}

string s_cat(string a, string b, arena * perm) {
    string s = a;
    if (!s.data || s.data + s.len != perm->beg) {
        s = s_clone(a, perm);
    }
    s.len += s_clone(b, perm).len;
    return s;
}

string s_reverse(string s) {
    char buf[s.len];
    memcpy(buf, s.data, s.len);
    for (size i = 0; i < s.len; i++) {
        s.data[i] = buf[s.len - 1 - i];
    }
    return s;
}

strings s_split(string s, char delim, arena * a) {
    strings vec = {0};
    size len = 0;
    size pos = 0;
    for (size i = 0; i < s.len; i++, len++) {
        if (s.data[i] == delim) {
            *push(&vec, a) = (string) {
                .data = s.data + pos,
                .len = len,
            };
            len = 0;
            pos = ++i;
        }
    }
    if (len != 0) {
        *push(&vec, a) = (string) {
            .data = s.data + pos,
            .len = len,
        };
    }
    return vec;
}


//bob

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

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

int run2(command * c, int fdout) {
    int pid = fork();
    if (pid == 0) {
        dup2(fdout, STDOUT_FILENO);
        close(fdout);
        return execv(c->data[0], c->data);
    }

    c->len = 0;
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
        if (c.data[i] != NULL) {
            printf("%s ", c.data[i]);
        }
    }
    printf("\n");
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

//files
string read_file(char * filename, arena * a) {
    FILE* f;
    f = fopen(filename, "rb");
    assert(f);

    fseek(f, 0L, SEEK_END);
    size filesize = 0;
    filesize = ftell(f);
    if (filesize < 0) {
        LOG_ERROR("Failed to parse file length");
        exit(EXIT_FAILURE);
    } else if (filesize == 0) {
        LOG_ERROR("File is empty");
        exit(EXIT_SUCCESS);
    }
    fseek(f, 0L, SEEK_SET);

    string temp = {0};
    temp.data = new(a, char, filesize);
    temp.len = filesize;

    int res = fread(temp.data, 1, filesize, f);
    if (res != filesize) {
        LOG_ERROR("Failed to parse file contents");
        exit(EXIT_FAILURE);
    }
    fclose(f);
    return temp;
}

#endif
