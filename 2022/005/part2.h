#include <aoc_helpers.h>
#include <aoc_2d_engine.h>

#include <aoc_stack.h>
#include <aoc_linked_list.h>
#include <aoc_parser.h>
#include <stdint.h>
#include "partx.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct context
{
    aoc_2d_eng_h _eng;
    struct dll_head _grippedBoxes;
    struct dll_head _parsers;
    struct dll_head _columns;
    struct dll_head _BoxStack;
    struct dll_head _cmds;
    coord_t _pos;
    char *spelling;
    size_t _result;
}context_t; 
typedef context_t *aoc_context_h;

#define CTX_CAST(_p) ((struct context *)_p)


static int crate_lift(aoc_context_h _ctx, command_t *_cmd);
static int crate_change_lane(aoc_context_h _ctx, command_t *_cmd);
static int crate_deposit(aoc_context_h _ctx, command_t *_cmd);
static int crane_action(aoc_context_h _ctx);

static void aoc_spell_ans(struct solutionCtrlBlock_t *_blk);