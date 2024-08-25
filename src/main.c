/*
 * Html parser entry point
 * 08/24/2024
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct filebuffer {
    size_t length;
    char* data;
} filebuffer;

filebuffer* loadfile(const char* filename) {
    FILE* fd = fopen(filename, "rb");
    if (!fd) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }

    filebuffer* fb = malloc(sizeof(filebuffer));

    fseek(fd, 0L, SEEK_END);
    fb->length = ftell(fd);
    rewind(fd);

    fb->data = malloc(sizeof(char) * (fb->length+1));

    int c, i = 0;
    do {
        c = getc(fd);
        fb->data[i] = c;
        i++;
    } while(c != EOF);

    fb->data[i] = '\0';
    return fb;
}


int main(int argc, char* argv[]) {
    fprintf(stdout, "%d\n", argc);

    if (argc > 1) {
        filebuffer* file = loadfile(argv[1]);
        fprintf(stdout, "Content:\n%s\n", file->data);
    }

    return EXIT_SUCCESS;
}


