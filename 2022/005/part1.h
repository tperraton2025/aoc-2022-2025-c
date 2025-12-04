#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_linked_list.h>
#include <stdint.h>
#include "partx.h"
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    aoc_2d_engine_h _eng;
    aoc_2d_object_h _grippedBox;
    struct dll_head _columns;
    struct dll_head _parsers;
    coord_t _pos;
    struct dll_head _cmds;
    char *spelling;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)
  

static int crate_lift(aoc_context_h _ctx, command_t *_cmd);
static int crate_change_lane(aoc_context_h _ctx, command_t *_cmd);
static int crate_deposit(aoc_context_h _ctx, command_t *_cmd);
static int crane_action(aoc_context_h _ctx);

static void aoc_spell_ans(struct solutionCtrlBlock_t *_blk);

