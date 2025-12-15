#include "partx.h"

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    TRY_TYPE_MALLOC(_blk->_data, struct context);
    if (!_blk->_data)
        return ENOMEM;
    context_h ctx = CTX_CAST(_blk->_data);
    parser_append(&ctx->_parsers, &float3Dparser, ctx);

    ctx->_segments = 1LU;
    ctx->_regions = dyn3d1malloc(ctx->_segments, freeregion);
    ctx->_result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);
    parse_all(&ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);
    dispatchpoints(ctx);
    measuredistances(ctx);
    size_t missing = printregions(ctx);

    aoc_info("raw    list has size %*.1lu", 3, ctx->_rawjboxlist._size);
    aoc_err("not measured         %3.1lu", missing);

    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);

    dll_free_all(&ctx->_rawjboxlist, free);
    dll_free_all(&ctx->_distances, free);
    dyn3dfree(ctx->_regions);
    free(ctx->highest);
    free(ctx->lowest);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;