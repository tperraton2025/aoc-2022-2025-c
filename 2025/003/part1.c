#include "partx.h"

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);

    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);
    _ctx->_tofind = 2;
    _ctx->_result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);

    dll_head_init(&_ctx->_originbyrating);
    dll_head_init(&_ctx->_originbyorder);

    populatejoltages(_ctx, _blk->_str);

    _ctx->_root = joltagetreechoice(NULL, &_ctx->_originbyrating, (joltage_h)_ctx->_originbyrating._first, freejoltchoice);
    if (!_ctx->_root)
        return EINVAL;

    _ctx->_conversion = base_10_conversion(0);
    _ctx->_root->_availablebyrating = &_ctx->_originbyrating;
    joltagepickallfeasible(_ctx->_root);

    dll_free_all(&_ctx->_originbyrating, free);
    dll_free_all(&_ctx->_originbyorder, free);

    aoc_tree_free(&_ctx->_root->_treenode);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;
