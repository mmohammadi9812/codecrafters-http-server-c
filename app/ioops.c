#include "ioops.h"


DIR* my_chdir(char* directory) {
    DIR* dp;
    if ((dp = opendir(directory)) == NULL) {
        printf("Failed to open %s: %s\n", directory, strerror(errno));
        exit(EXIT_FAILURE);
    }
    chdir(directory);
    return dp;
}

FILE* find_file(char* base_dir, char* name) {
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

void read_file(FILE* f, long* size, char** buf) {
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *buf = calloc(*size + 1, sizeof(char));
    fread(*buf, sizeof(char), *size, f);
    fclose(f);
}

void write_file(char* file_name, int size, char *buffer) {
    FILE* f = fopen(file_name, "w");
    if (f == NULL) {
        printf("Failed to open file %s for writing\n", file_name);
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, sizeof(char), size, f);
    fclose(f);
}

char* my_compress(char* input) {
    int size = strlen(input);
    char *output = calloc(size+1, sizeof(char));
    z_stream out_stream;
    out_stream.zalloc = Z_NULL;
    out_stream.zfree = Z_NULL;
    out_stream.opaque = Z_NULL;

    out_stream.avail_in = (uInt)strlen(input)+1; // size of input, string + terminator
    out_stream.next_in = (Bytef *)input; // input char array
    out_stream.avail_out = (uInt)sizeof(output); // size of output
    out_stream.next_out = (Bytef *)output; // output char array

    // the actual compression work.
    deflateInit(&out_stream, Z_BEST_COMPRESSION);
    deflate(&out_stream, Z_FINISH);
    deflateEnd(&out_stream);
    return output;
}
