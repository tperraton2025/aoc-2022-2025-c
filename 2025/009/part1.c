#include "partx.h"

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    TRY_TYPE_MALLOC(_blk->_data, struct context);
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);

    coord_t _prelcoordmaxima = {._x = 2, ._y = 2};
    _ctx->_eng = aoc_2d_eng_create(&_prelcoordmaxima, '.', 0LU, objh_byXfirst, false);
    parser_append(&_ctx->_parsers, &blockparser, _ctx);
    aoc_2d_eng_parse_cli(_ctx->_eng, argc, argv);

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
    aoc_2d_eng_draw(ctx->_eng);
    
    ctx->_blocksby_x = aoc_2d_eng_list_obj_pos(ctx->_eng, NULL);
    ctx->_blocksby_y = aoc_2d_eng_list_obj_pos(ctx->_eng, posh_byYfirst);


    size_t evaluations = 0;

    LL_FOREACH_P_REV(last, ctx->_blocksby_y)
    {
        coord_t frmlstpos = ((coord_tracker_h)last)->_coord; 
        LL_FOREACH_P(frst, ctx->_blocksby_y)
        {
            coord_t frmfstpos = ((coord_tracker_h)frst)->_coord; 
            evaluations++;

            size_t newarea = (HIGHEST(frmfstpos._x, frmlstpos._x) - LOWEST(frmfstpos._x, frmlstpos._x) + 1LU) *
                             (HIGHEST(frmfstpos._y, frmlstpos._y) - LOWEST(frmfstpos._y, frmlstpos._y) + 1LU);

            if (ctx->_result < newarea)
            {
                printf("-- area from %s ", strpos(&frmlstpos));
                printf("and %s  ", strpos(&frmfstpos));
                printf("is %lu after eval#%lu\n", newarea, evaluations);
                ctx->_result = HIGHEST(ctx->_result, newarea);
            }
        }
    }

    dll_free_all(ctx->_blocksby_x, free);
    dll_free_all(ctx->_blocksby_y, free);
    free(ctx->_blocksby_x);
    free(ctx->_blocksby_y);
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

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;