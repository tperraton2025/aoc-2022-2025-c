#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_numeric.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

typedef struct
{
    struct dll_head _params[16];
    dll_node_h _paramnodes[16];
    struct dll_head _ops;
    char **_instr;
    size_t _instrcnt;
    char *_opstr;
    size_t _line;
} problem_t;

extern parser_t numparser;
extern parser_t opsparser;
extern parser_t octonumparser;
extern parser_t octoopsparser;

typedef struct context
{
    struct dll_head parsers;
    problem_t _prob;
    size_t _result;
} context_t;

typedef context_t *context_h;

void checkparseconsistency(context_h _ctx);
void checkoctoparseconsistency(context_h _ctx);
size_t add(context_h _ctx);
size_t multiply(context_h _ctx);
size_t octoadd(context_h _ctx);
size_t octomult(context_h _ctx);