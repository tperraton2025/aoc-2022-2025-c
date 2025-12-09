#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct context
{
    aoc_2d_eng_h _eng;
    struct dll_head _parsers;
    struct dll_head _activebeams;
    coord_t _cursor;
    size_t _result;
} context_t;

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

int objectsparser(void *arga, char *str);
int initpropagation(context_h ctx);
int loopbeampropagations(context_h ctx);

extern parser_t _objparser;