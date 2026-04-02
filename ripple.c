#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>

#define FILE_BUF_SIZE 4096
#define PATH_BUF_SIZE 256

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
    if (stat(argv[1], &stat_buff) != 0 || !S_ISDIR(stat_buff.st_mode)) {
        fprintf(stderr, "[!] Invalid source path '%s'\n", argv[1]);
        return -1;
    }
    
    args->src = argv[1];
    args->dst = argv[2];
    return 0;
}

int copy_file(const char *src_path, const char *dst_path)
{
    FILE *src = fopen(src_path, "rb");
    if (!src) {
        fprintf(stderr, "[!] Unable to open file '%s' for reading\n", src_path);
        return -1;
    }

    FILE *dst = fopen(dst_path, "wb");
    if (!dst) {
        fprintf(stderr, "[!] Unable to open file '%s' for writing\n", dst_path);
        fclose(src);
        return -1;
    }

    unsigned char buf[FILE_BUF_SIZE];
    size_t n;

    while ((n = fread(buf, 1, FILE_BUF_SIZE, src)) > 0) {
        if (fwrite(buf, 1, n, dst) != n) {
            fprintf(stderr, "[!] Unable to write to file '%s'\n", dst_path);
            fclose(src);
            fclose(dst);
            return -1;
        }
    }

    int ok = feof(src) ? 0 : -1;
    fclose(src);
    fclose(dst);
    return ok;
}

int copy_directory(const char *src_path, const char *dst_path)
{
    if (mkdir(dst_path, 0755) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "[!] Unable to create directory '%s'\n", dst_path);
            return -1;
        }
    }

    DIR *dir = opendir(src_path);
    if (NULL == dir) {
        fprintf(stderr, "[!] Unable to open directory '%s'\n", src_path);
        return -1;
    }

    struct dirent *entry;
    char src_entry_path[PATH_BUF_SIZE];
    char dst_entry_path[PATH_BUF_SIZE];

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(src_entry_path, PATH_BUF_SIZE, "%s/%s", src_path, entry->d_name);
        snprintf(dst_entry_path, PATH_BUF_SIZE, "%s/%s", dst_path, entry->d_name);

        switch (entry->d_type) {
            case DT_DIR:
                if (copy_directory(src_entry_path, dst_entry_path) != 0) {
                    closedir(dir);
                    return -1;
                }

                break;
            default:
                if (copy_file(src_entry_path, dst_entry_path) != 0) {
                    closedir(dir);
                    return -1;
                }

                break;
        }
    }

    closedir(dir);
    return 0;
}

int main(int argc, char *argv[])
{
    args_t args;
    if (parse_args(argc, argv, &args))
        return -1;

    int result;
    if ((result = copy_directory(args.src, args.dst)) == 0)
        printf("Successfully executed.\n");

    return result;
}
