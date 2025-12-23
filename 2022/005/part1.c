#include "partx.h"

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = NULL;
    TRY_TYPE_MALLOC(_blk->_data, struct context);
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_result = 0;

    _ctx->_pos._x = 1;
    _ctx->_pos._y = 1;

    dll_head_init(&_ctx->_cmds);
    dll_head_init(&_ctx->_columns);
    dll_head_init(&_ctx->_parsers);

    parser_append(&_ctx->_parsers, &blockparser, _ctx);
    parser_append(&_ctx->_parsers, &commandparser, &_ctx->_cmds);

    coord_t _prelcoordmaxima = {._x = 2, ._y = 2};

    _ctx->_eng = aoc_2d_eng_create(&_prelcoordmaxima, '~', 0, objref_ydecr_xdecr, false);

    if (!_ctx->_eng)
        goto cleanup;

    aoc_2d_eng_parse_cli(_ctx->_eng, argc, argv);
    _ctx->_verbose = aoc_2d_eng_isdrawing(_ctx->_eng);
    return 0;

cleanup:
    FREE(_blk->_data);
error:
    return ENOMEM;
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
    aoc_2d_eng_extend_one_direction(_ctx->_eng, 75, AOC_2D_DIR_UP);
    aoc_2d_eng_draw(_ctx->_eng);
    
    crane_action(_ctx, false);
    
    aoc_spell_ans(_blk);
    _ctx->_verbose = true;
    
    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);

    LL_FOREACH(colnode, _ctx->_columns)
    {
        dll_free_all(&((columnnode_h)colnode)->_boxes, free);
    }
    dll_free_all(&_ctx->_columns, free);
    dll_free_all(&_ctx->_grippedBoxes, free);
    engine_free(_ctx->_eng);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;
