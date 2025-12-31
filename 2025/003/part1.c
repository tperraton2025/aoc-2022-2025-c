#include "partx.h"

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{

    _blk->_data = calloc(1LU, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);
    _ctx->_result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);

    dll_head_init(&_ctx->_originbyrating);
    dll_head_init(&_ctx->_originbyorder);

    populatejoltages(_ctx, _blk->_str);

    size_t _matchfound = 0;
    LL_FOREACH(_node, _ctx->_originbyrating)
    {
        _ctx->_root = joltagetreechoice(NULL, 1LU, &_ctx->_originbyrating, &_ctx->_originbyorder, (joltage_h)_node, freejoltchoice);
        if (!_ctx->_root)
            goto error;
        _matchfound = joltagepickallfeasible(_ctx->_root);

        aoc_tree_free_all(&_ctx->_root->_treenode);

        if (_matchfound)
        {
            _ctx->_result += _matchfound;
            dll_free_all(&_ctx->_originbyrating, free);
            dll_free_all(&_ctx->_originbyorder, free);
            break;
        }
    }
    return 0;

error:
    dll_free_all(&_ctx->_originbyrating, free);
    dll_free_all(&_ctx->_originbyorder, free);

    return 1;
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
