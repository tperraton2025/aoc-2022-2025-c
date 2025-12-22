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

typedef enum AXIS_E
{
    AXIS_X = 0LU,
    AXIS_Y,
    AXIS_MAX
}axis_t;
typedef struct context
{
    size_t _result;
    aoc_2d_eng_h _eng;
    dll_head_t _parsers;
    dll_head_h _blocks_xaligned;
    dll_head_h _blocks_yaligned;

    dll_head_t _boundaries;

    coord_t _cursor;
    bool _verbose;
} context_t;

typedef context_t *context_h;

int objectsparser(void *arg, char *str);
int scanboundaries(context_h ctx );
void mergeboundaries(context_h ctx);
void plotboundaries(context_h ctx);

void populateedges(context_h const ctx, const axis_t axis );
static int trackcoord(coord_t *goal, coord_t *curs);
dll_node_h stopatlastonsame_axis(dll_node_h start, size_t offset, size_t axisval);
void get_axis_extremesfromslice(dll_node_h start, dll_node_h end, size_t axis, coordpair_h thr);

extern parser_t blockparser;