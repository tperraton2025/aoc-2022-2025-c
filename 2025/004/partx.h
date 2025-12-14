#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    aoc_2d_eng_h _eng;
    dll_head_h _rollspositions;
    struct dll_head _parsers;
    size_t _result;
};
typedef struct context *aoc_context_h;

int parserolls(void *arg, char *str);
void markfreerolls(struct context *_ctx);
size_t trimaccessiblepositions(aoc_2d_eng_h eng, dll_head_h allpos);

extern struct parser blockparser;