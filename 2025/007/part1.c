#include "partx.h"

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);
    _ctx->_result = 0;

    coord_t _lim = {._x = 2, ._y = 2};
    _ctx->_eng = aoc_2d_eng_create(&_lim, '.', 0);
    parser_append(&_ctx->_parsers, &_objparser, _ctx);
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    aoc_2d_eng_draw(_ctx->_eng);



    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    (void)_ctx;
    engine_free(_ctx->_eng);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;