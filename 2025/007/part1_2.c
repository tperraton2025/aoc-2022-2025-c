#include "partx.h"

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = calloc(1LU, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);
    _ctx->_result = 0;

    coord_t _lim = {._x = 2, ._y = 2};
    _ctx->_eng = aoc_2d_eng_create(&_lim, '.', 10, objref_ydecr_xdecr, true);
    parser_append(&_ctx->_parsers, &_objparser, _ctx);
    aoc_2d_eng_parse_cli(_ctx->_eng, argc, argv);

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
    sizepair_t ans = {0};
    aoc_2d_eng_h eng = ctx->_eng;
    int ret = 0;

    aoc_2d_eng_extend_one_direction(eng, 2, AOC_2D_DIR_RIGHT);
    aoc_2d_eng_extend_one_direction(eng, 2, AOC_2D_DIR_LEFT);

    ctx->_allobjs = aoc_2d_eng_list_objects(eng, NULL, NULL);
    ctx->_allspliters = aoc_2d_eng_list_objects(eng, "^", "S");

    size_t progress = 0;
    LL_FOREACH_P(objn, ctx->_allobjs)
    {
        aoc_2d_obj_h objh = (aoc_2d_obj_h)((aoc_2d_obj_ref_h)objn)->data;
        beamexplore(ctx, objh);
        ans = memoreport(ctx, objh, progress);
        progress++;
    }

    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ans._0);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, "007 part 2", ans._1);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);
    dll_free_all(&ctx->_activebeams, free);
    dll_free_all(ctx->_allspliters, free);
    free(ctx->_allspliters);
    DYN2DFREE_DEALLOC(ctx->_memosplits,
                      memospawn_h,
                      ctx->limits._max._x + 1,
                      ctx->limits._max._y + 1,
                      freememo);
    dll_free_all(ctx->_allobjs, free);
    free(ctx->_allobjs);
    engine_free(ctx->_eng);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;
