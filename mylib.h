#ifndef MYLIB
#define MYLIB

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

#define max(a,b) ((a) > (b) ? (a) : (b))

int myrand(int min, int max);
size_t ceil_to_power_of_two(size_t size);
void print_bin(FILE* output, unsigned int x);

#endif