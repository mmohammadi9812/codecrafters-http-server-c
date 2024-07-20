#ifndef __IOOPS_H__
#define __IOOPS_H__
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>


DIR* my_chdir(char*);
FILE* find_file(char* base_dir, char* name);
void read_file(FILE*, long*, char**);
void write_file(char*, int, char*);
char* my_compress(char*);

#endif
