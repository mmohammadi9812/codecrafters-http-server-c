#ifndef __IOOPS_H__
#define __IOOPS_H__
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

DIR* my_chdir(char*);
FILE* find_file(char* base_dir, char* name);
void read_file(FILE*, long*, char**);
void write_file(char*, int, char*);
char* my_compress(char*);
char* ltrim(char*);

#endif
