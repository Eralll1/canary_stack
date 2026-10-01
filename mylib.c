#include "mylib.h"

int myrand(int min, int max){
    return rand() % (max - min + 1) + min;
}

size_t ceil_to_power_of_two(size_t size){
    size_t res = 1;
    while (res <= size){
        res <<= 1;
    }
    return res;
}

void print_bin(FILE* output, unsigned int x) {
    for (int i = (int)(sizeof x * CHAR_BIT) - 1; i >= 0; i--)
        fprintf(output, "%c", (x >> i) & 1u ? '1' : '0');
}