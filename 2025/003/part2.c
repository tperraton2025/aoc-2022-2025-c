#include "partx.h"

struct context
{
    joltchoice_h _origin;
    joltchoice_h _current;
    long long unsigned result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);
    _ctx->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_origin = newjoltchoice(NULL);
    _ctx->_current = _ctx->_origin;

    populatejoltageratings(_ctx->_origin->_joltages, _blk->_str);
    joltage_h _jolth = (joltage_h)_ctx->_origin->_joltages->_first;

    joltchoice_h _subchoice = makenextjoltchoice(_ctx->_current, _jolth);

    aoc_ans("result for %s %lu", strtok(_blk->_str, "\n"), _ctx->_origin->_conversion->_val);

    _ctx->result += _ctx->_origin->_conversion->_val;
    aoc_tree_free(&_ctx->_origin->_treenode);
    free(_ctx->_origin);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    aoc_ans("AOC %s %s solution is %llu", CONFIG_YEAR, _blk->_name, _ctx->result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;