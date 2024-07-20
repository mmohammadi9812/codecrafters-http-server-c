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

char*
my_compress(char* input) {
    int inputSize = strlen(input), outputSize = inputSize + 1;
    char* output = calloc(outputSize, sizeof(char));
    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = (uInt)inputSize;
    zs.next_in = (Bytef*)input;
    zs.avail_out = (uInt)outputSize;
    zs.next_out = (Bytef*)output;

    // hard to believe they don't have a macro for gzip encoding, "Add 16" is the best thing zlib can do:
    // "Add 16 to windowBits to write a simple gzip header and trailer around the compressed data instead of a zlib wrapper"
    deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
    deflate(&zs, Z_FINISH);
    deflateEnd(&zs);
    return output;
}
