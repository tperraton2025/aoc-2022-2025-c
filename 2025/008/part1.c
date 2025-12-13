#include "partx.h"

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    TRY_TYPE_MALLOC(_blk->_data, struct context);
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);
    parser_append(&_ctx->_parsers, &float3Dparser, _ctx);

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
    aoc_info("sorted list has size %*.lu", 3, ctx->_sortedjboxlist._size);
    aoc_info("raw    list has size %*.lu", 3, ctx->_rawjboxlist._size);

    // sortdistances(&ctx->_rawjboxlist, &ctx->_distances);
    // printdlist(&ctx->_distances, BGREEN);
    DYNCUBICMALLOC(ctx->_regions, dll_head_h, (ctx->_segments + 3));
    printplist(&ctx->_sortedjboxlist, BGREEN);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    dll_free_all(&_ctx->_rawjboxlist, free);
    dll_free_all(&_ctx->_sortedjboxlist, free);
    dll_free_all(&_ctx->_distances, free);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;