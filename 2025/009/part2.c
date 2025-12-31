#include "partx.h"

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = calloc(1LU, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);

    coord_t _prelcoordmaxima = {._x = 2, ._y = 2};
    _ctx->_eng = aoc_2d_eng_create(&_prelcoordmaxima, '.', 0LU, objref_xdecr_ydecr, false);
    parser_append(&_ctx->_parsers, &blockparser, _ctx);
    aoc_2d_eng_parse_cli(_ctx->_eng, argc, argv);

    _ctx->_verbose = true;
    for (size_t iit = 0; iit < argc; iit++)
    {
        if (!strcmp("--no-draw", argv[iit]))
            _ctx->_verbose = false;
    }
    _ctx->_result = 0;
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
    context_h ctx = CTX_CAST(_blk->_data);

    ctx->_blocks_xaligned = aoc_2d_eng_list_obj_pos(ctx->_eng, NULL);
    ctx->_blocks_yaligned = aoc_2d_eng_list_obj_pos(ctx->_eng, posnode_ydecr_xdecr);

    size_t evaluations = 0;

    aoc_2d_eng_draw(ctx->_eng);
    scanboundaries(ctx);
    
    mergeboundaries(ctx);
    plotboundaries(ctx);

    dll_free_all(ctx->_blocks_xaligned, free);
    dll_free_all(ctx->_blocks_yaligned, free);
    dll_free_all(&ctx->_boundaries, free);
    free(ctx->_blocks_xaligned);
    free(ctx->_blocks_yaligned);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    (void)_ctx;
    engine_free(_ctx->_eng);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;