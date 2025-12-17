#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_parser.h>
#include <aoc_2d_engine.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct context
{
    size_t _result;
    aoc_2d_eng_h _eng;
    dll_head_t _parsers;
    dll_head_h _blocksby_x;
    dll_head_h _blocksby_y;

    coord_t _cursor;
} context_t;

typedef context_t *context_h;

int objectsparser(void *arg, char *str);

extern parser_t blockparser;