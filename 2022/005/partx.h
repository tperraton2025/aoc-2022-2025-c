
#define SLEEP_TIME_MS (0)
#include <aoc_parser.h>
#include <stdlib.h>
#include <string.h>

#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_linked_list.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct context
{
    aoc_2d_eng_h _eng;
    coord_t _pos;
    char *spelling;

    dll_head_t _cmds;
    dll_head_t _columns;
    dll_head_t _parsers;
    dll_head_t _grippedBoxes;
    dll_head_t _BoxStack; 
    bool _verbose;

    size_t _result;
} context_t;

typedef context_t *aoc_context_h;

typedef_dllnode(column, size_t _col_index; size_t _col_x; dll_head_t _boxes;);
typedef_dllhandle(columnnode);

#define CTX_CAST(_p) ((struct context *)_p)

typedef struct command
{
    struct dll_node _node;
    size_t count;
    size_t from;
    size_t to;
} command_t;

static command_t *command_ctor(command_t *_ncmd)
{
    command_t *ret = calloc(1LU, sizeof(struct command));
    if (!ret)
        return NULL;
    ret->count = _ncmd->count;
    ret->from = _ncmd->from;
    ret->to = _ncmd->to;
    return ret;
}

int parseblock(void *arg, char *_str);
int parsecommand(void *arg, char *_str);

int crate_lift(struct context *_ctx, command_t *_cmd, size_t boxestolift);
int crate_change_lane(aoc_context_h _ctx, command_t *_cmd);
int crate_deposit(struct context *_ctx, command_t *_cmd, size_t boxestodeposit);
int crane_action(aoc_context_h _ctx, bool multiboxmode);

void aoc_spell_ans(struct solutionCtrlBlock_t *_blk);

bool column_hassame_x(void *arga, void *argb);
bool column_hassame_index(void *arga, void *argb);
dll_node_h column_haslower_x(dll_node_h arga, dll_node_h argb);

static struct parser blockparser = {._name = "block", ._func = parseblock};
static struct parser commandparser = {._name = "command", ._func = parsecommand};
