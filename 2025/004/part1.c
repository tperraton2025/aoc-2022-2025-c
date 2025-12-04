#include "partx.h"

struct context
{
    aoc_2d_engine_h _engine;
    dll_head_t _parsers;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);
    dll_head_init(&_ctx->_parsers);
    
    coord_t _max_xy = {1, 1};
    _ctx->_engine = engine_create(&_max_xy, '.', 0);
    
    parser_append(&_ctx->_parsers, &blockparser, _ctx->_engine);
    _ctx->result = 0;
    
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

    engine_draw(_ctx->_engine);
    int result = _ctx->result;
    aoc_ans("AOC %s %s solution is %i", CONFIG_YEAR, _blk->_name, result);
    return result;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    (void)_ctx;
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;