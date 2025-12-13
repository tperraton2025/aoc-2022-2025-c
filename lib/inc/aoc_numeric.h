#ifndef AOC_NUMERIC_H
#define AOC_NUMERIC_H
#include <aoc_helpers.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

#define SQUARE(_a) ((_a) * (_a))
typedef struct base10
{
    char _digits[64];
    size_t _lastdigit;
    size_t _val;
} base10_t;
typedef struct base10 *base10_h;

void print_base10(base10_h base10);
base10_h base_10_conversion(size_t luint);
int base_10_increment(base10_h base10);
size_t base_10_reverse_conversion(base10_h base10);
void base_10_clear(base10_h base10);

char *string_base10(base10_h base10);
char *strconv(base10_h base10);

char *strrev(char *_str);
#endif