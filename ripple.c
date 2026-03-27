#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUF_SIZE 4096

typedef struct {
    const char *src;
    const char *dst;
} args_t;

int parse_args(int argc, char *argv[], args_t *args) 
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
        return -1;
    }

    struct stat stat_buff;
    for (size_t i = 1; i < argc; i++) {
        if (stat(argv[i], &stat_buff) != 0) {
            fprintf(stderr, "[!] Invalid path '%s'\n", argv[i]);
            return -1;
        }
    }

    args->src = argv[1];
    args->dst = argv[2];
    return 0;
}

int copy_file(const char *src_path, const char *dst_path) 
{
    FILE *src = fopen(src_path, "rb");
    if (!src) {
        fprintf(stderr, "[!] Unable to open '%s' for reading\n", src_path);
        return EXIT_FAILURE;
    }

    FILE *dst = fopen(dst_path, "wb");
    if (!dst) { 
        fprintf(stderr, "[!] Unable to open '%s' for writing\n", dst_path);
        fclose(src); 
        return -1; 
    }

    unsigned char buf[BUF_SIZE];
    size_t n;

    while ((n = fread(buf, 1, BUF_SIZE, src)) > 0) {
        if (fwrite(buf, 1, n, dst) != n) {
            fprintf(stderr, "[!] Unable to write to '%s', aborting\n", dst_path);
            fclose(src);
            fclose(dst);
            return EXIT_FAILURE;
        }
    }

    int ok = feof(src) ? 0 : -1;
    fclose(src);
    fclose(dst);
    return ok;
}

int main(int argc, char *argv[])
{
    args_t args;
    if (parse_args(argc, argv, &args))
        return EXIT_FAILURE;

    if (copy_file(args.src, args.dst) != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
