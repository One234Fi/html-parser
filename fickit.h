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
    size cap;
} string;

typedef struct string_buffer {
    char * data;
    size len;
    size cap;
} string_buffer;

#define String(s) (string){(char*)s, strlen(s), 0}
bool s_equal(string a, string b);
#define s_equal_c(s, cstr) s_equal(s, String(cstr))
bool s_equal_ignore_case(string a, string b);
string s_clone(string s, arena * a);
string s_cat(string a, string b, arena * perm);
string s_reverse(string s);


//Optionals
typedef struct {
    void * val;
    bool exists;
} opt;

typedef opt opt_str;
typedef opt opt_i32;

#define opt_get(opt, type) ((type *) ((opt)->val))
#define opt_unwrap(opt, type, or) ((opt)->exists ? (opt_get(opt, type)) : or)
#define opt_str_equal_c(optstr, cstr) opt_str_equal_s(optstr, String(cstr))
bool opt_str_equal_s(opt_str* opt_s, string s);
void opt_str_append(opt_str* s, arena * a, char c);

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
            printf("%s", a->debug_name);
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


//Optionals

void opt_str_append(opt_str* s, arena * a, char c) {
    if(!s->exists) {
        s->val = new(a, string);
        s->exists = true;
    }
    *push(opt_get(s, string), a) = c;
}

bool opt_str_equal_s(opt_str* opt_s, string s) {
    if (opt_s->exists) {
        return s_equal(*opt_get(opt_s, string), s);
    }
    return false;
}


#endif
