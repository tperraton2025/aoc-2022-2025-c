#include "partx.h"

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = malloc(sizeof(struct context));
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);

    dll_head_init(&_ctx->_parsers);

    coord_t _max_xy = {1, 1};
    _ctx->_eng = aoc_2d_eng_create(&_max_xy, '.', 0, bycoordinatesYfirst, false);
    aoc_2d_eng_disable_draw(_ctx->_eng);
    parser_append(&_ctx->_parsers, &blockparser, _ctx->_eng);
    _ctx->_result = 0;

    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_rollspositions = aoc_2d_eng_list_obj_pos(_ctx->_eng);
    size_t liberated = trimaccessiblepositions(_ctx->_eng, _ctx->_rollspositions);
    while (liberated)
    {
        _ctx->_result += liberated;
        liberated = trimaccessiblepositions(_ctx->_eng, _ctx->_rollspositions);
    }

    if (43 == _ctx->_result || 8493 == _ctx->_result)
    {
        aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    }
    else
    {
        aoc_err("AOC %s %s solution is not %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    }

    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    engine_free(_ctx->_eng);

    dll_free_all(_ctx->_rollspositions, free);
    free(_ctx->_rollspositions);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;
