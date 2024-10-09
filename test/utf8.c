#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <limits.h>

void print_utf8_file();
void print_ascii_file();



size_t bytes_in_char(unsigned char c) {
    if (c < 128) {
        return 0;
    }

    if (c < 224) {
        return 1;
    }

    if (c < 240) {
        return 2;
    }

    return 3;
}

void print_utf8_file() {
    char* filename = "testdata/archlinux.html";
    FILE* fd = fopen(filename, "rb");
    if (!fd) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }


    fseek(fd, 0L, SEEK_END);
    size_t byte_count = ftell(fd);
    rewind(fd);

    wchar_t contents [byte_count];
    memset(contents, 0, byte_count * sizeof(wchar_t));
    int c;
    size_t index = 0;
    while ((c = fgetc(fd)) != EOF) {
        wchar_t t = c;
        for (size_t i = 0; i < bytes_in_char(c); i++) {
            t += fgetc(fd);
        }
        contents[index] = t;
        index++;
    }
    contents[index] = L'\0';

    setlocale(LC_ALL, "C.UTF-8");
    fprintf(stdout, "%ls\n", contents);
}

void print_ascii_file() {
    char* filename = "testdata/include-html-tag.html";
    FILE* fd = fopen(filename, "rb");
    if (!fd) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }


    fseek(fd, 0L, SEEK_END);
    size_t byte_count = ftell(fd);
    rewind(fd);

    wchar_t contents [byte_count];
    memset(contents, 0, byte_count * sizeof(wchar_t));
    int c;
    size_t index = 0;
    while ((c = fgetc(fd)) != EOF) {
        wchar_t t = c;
        for (size_t i = 0; i < bytes_in_char(c); i++) {
            t += fgetc(fd);
        }
        contents[index] = t;
        index++;
    }
    contents[index] = L'\0';

    setlocale(LC_ALL, "C.UTF-8");
    fprintf(stdout, "%ls\n", contents);
}
