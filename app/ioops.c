#include "ioops.h"

DIR*
my_chdir(char* directory) {
    DIR* dp;
    if ((dp = opendir(directory)) == NULL) {
        printf("Failed to open %s: %s\n", directory, strerror(errno));
        exit(EXIT_FAILURE);
    }
    chdir(directory);
    return dp;
}

FILE*
find_file(char* base_dir, char* name) {
    struct dirent* entry;
    DIR* dp = my_chdir(base_dir);
    int found = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (strncmp(entry->d_name, name, strlen(name)) == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return NULL;
    }
    return fopen(entry->d_name, "r");
}

void
read_file(FILE* f, long* size, char** buf) {
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *buf = calloc(*size + 1, sizeof(char));
    fread(*buf, sizeof(char), *size, f);
    fclose(f);
}

void
write_file(char* file_name, int size, char* buffer) {
    FILE* f = fopen(file_name, "w");
    if (f == NULL) {
        printf("Failed to open file %s for writing\n", file_name);
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, sizeof(char), size, f);
    fclose(f);
}

char*
ltrim(char* s) {
    while (isspace(*s)) {
        s++;
    }
    return s;
}

void
gzip_compress(const char* input, size_t input_size, unsigned char** output, size_t* output_size) {
    z_stream strm;
    int ret;
    unsigned char out_buffer[8192];

    *output = NULL;
    *output_size = 0;

    memset(&strm, 0, sizeof(strm));
    ret = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        perror("[-] deflateInit2");
    }

    strm.next_in = (unsigned char*)input;
    strm.avail_in = input_size;

    do {
        strm.next_out = out_buffer;
        strm.avail_out = sizeof(out_buffer);

        ret = deflate(&strm, Z_FINISH);
        if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR) {
            deflateEnd(&strm);
            perror("[-] deflate != Z_OK");
        }

        size_t have = sizeof(out_buffer) - strm.avail_out;
        *output = realloc(*output, *output_size + have);
        if (*output == NULL) {
            deflateEnd(&strm);
            perror("[-] realloc");
        }

        memcpy(*output + *output_size, out_buffer, have);
        *output_size += have;
    } while (strm.avail_out == 0);

    deflateEnd(&strm);
}
