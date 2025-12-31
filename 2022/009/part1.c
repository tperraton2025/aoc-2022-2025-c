#include "partx.h"

typedef struct context
{
    aoc_2d_eng_h _eng;
    aoc_2d_obj_h _head;
    aoc_2d_obj_h _tail;

    aoc_2d_obj_h _lastMovedLink;
    coord_t _prevPosFromLastMovedLink;
    struct dll_head _movs;
    struct dll_head _tailPos;
    size_t _result;
} context_t;
typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

#define MOV_CAST(_p) ((movement_t *)_p)
#define MAP_MID_SIZE (1024)

static int track_tail(context_h ctx, coord_t *_pos)
{
    coordnode_h _npos = NULL;
    _npos = malloc(sizeof(coordnode_t));
    if (!_npos)
        return ENOMEM;

    _npos->_coord._x = _pos->_x;
    _npos->_coord._y = _pos->_y;
    if (NULL == dll_node_find_by_property(&ctx->_tailPos, (void *)_npos, coordnodes_equal))
        return dll_node_append(&ctx->_tailPos, NODE_CAST(_npos));
    else
        FREE(_npos);
    return EALREADY;
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    int ret = 0;
    _blk->_data = calloc(1LU, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h ctx = CTX_CAST(_blk->_data);

    ctx->_result = 0;

    coord_t _prelcoordmaxima = {._x = 20, ._y = 20};

    dll_head_init(&ctx->_movs);
    dll_head_init(&ctx->_tailPos);

    ctx->_eng = aoc_2d_eng_create(&_prelcoordmaxima, '.', 0, objref_ydecr_xdecr, false);
    if (!ctx->_eng)
        goto error;

    aoc_2d_eng_extend_one_direction(ctx->_eng, 10, AOC_2D_DIR_UP);
    aoc_2d_eng_extend_one_direction(ctx->_eng, 10, AOC_2D_DIR_RIGHT);
    aoc_2d_eng_parse_cli(ctx->_eng, argc, argv);

    coord_t start = {0};

    aoc_2d_eng_get_canvas_center(ctx->_eng, &start);

    ctx->_head = aoc_2d_obj_ctor(ctx->_eng, "head", &start, "H", OBJ_FLAG_NO_COLLISION | OBJ_FLAG_MOBILE, "");
    ctx->_tail = aoc_2d_obj_ctor(ctx->_eng, "tail", &start, "T", OBJ_FLAG_NO_COLLISION | OBJ_FLAG_MOBILE, "");

    if (!ctx->_head || !ctx->_tail)
    {
        ret = ENOMEM;
        goto error;
    }

    ret = aoc_2d_eng_append_obj(ctx->_eng, ctx->_head);
    if (ret)
        goto error;
    ret = aoc_2d_eng_append_obj(ctx->_eng, ctx->_tail);
    if (ret)
        goto error;

    track_tail(ctx, &start);
    aoc_2d_eng_prompt_obj_list(ctx->_eng);
    return 0;

error:
    if (ctx->_eng)
        engine_free(ctx->_eng);
    else
    {
        if (ctx->_head)
            FREE(ctx->_head);
        if (ctx->_tail)
            FREE(ctx->_tail);
    }
    FREE(_blk->_data);
    return ret;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    char dir = 'R';
    context_h ctx = CTX_CAST(_blk->_data);
    size_t cnt = 0;
    movement_t *nmov = NULL;
    if (2 == sscanf(_blk->_str, "%c %ld", &dir, &cnt))
    {
        nmov = calloc(1LU, sizeof(movement_t));
        if (!nmov)
            return ENOMEM;
        nmov->steps = cnt;
        nmov->dir = char_to_AOC_2D_DIR(&dir);
        if (nmov->dir >= AOC_2D_DIR_MAX)
            goto error;

        if (dll_node_append(&ctx->_movs, NODE_CAST(nmov)))
            goto error;
        return 0;
    error:
        aoc_err("Error parsing %s", _blk->_str);
        FREE(nmov);
        return 1;
    }
    return 0;
}

static void int_refresh_link(struct solutionCtrlBlock_t *_blk, aoc_2d_obj_h _head, aoc_2d_obj_h _tail, AOC_2D_DIR _dir)
{
    context_h ctx = CTX_CAST(_blk->_data);
    coordnode_h _npos = NULL;

    if (ctx->_head == _head)
    {
        ctx->_lastMovedLink = _head;
        ctx->_prevPosFromLastMovedLink = *aoc_2d_obj_get_pos(ctx->_lastMovedLink);
        if (aoc_2d_eng_step_obj(ctx->_eng, _head, 1LU, _dir, NULL))
            aoc_2d_eng_prompt_extra_stats_as_err(ctx->_eng, "collision while moving %s", aoc_2d_obj_name(_head));
    }
    if (1 < aoc_2d_eng_get_dist_between_objects(ctx->_eng, _head, _tail))
    {
        if (aoc_2d_eng_put_obj_and_redraw(ctx->_eng, _tail, ctx->_prevPosFromLastMovedLink))
            aoc_2d_eng_prompt_extra_stats_as_err(ctx->_eng, "collision while moving %s", aoc_2d_obj_name(_tail));
        aoc_2d_eng_draw_obj(ctx->_eng, _tail, "");
        coord_t _npos = *aoc_2d_obj_get_pos(_tail);
        ctx->_prevPosFromLastMovedLink = *aoc_2d_obj_get_pos(_tail);
        track_tail(ctx, &ctx->_prevPosFromLastMovedLink);
        ctx->_lastMovedLink = _tail;
    }
    aoc_2d_eng_prompt_obj_list(ctx->_eng);
    engine_cursor_user_stats(ctx->_eng);
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    movement_t *nmov = NULL;
    context_h ctx = CTX_CAST(_blk->_data);
    size_t delay = aoc_2d_eng_getrefreshdelay(ctx->_eng);
    aoc_2d_eng_draw(ctx->_eng);
    LL_FOREACH(_node, ctx->_movs)
    {
        nmov = MOV_CAST(_node);
        for (size_t ii = 0; ii < nmov->steps; ii++)
        {
            aoc_2d_eng_setrefreshdelay(ctx->_eng, 0LU);
            int_refresh_link(_blk, ctx->_head, ctx->_tail, nmov->dir);
            aoc_2d_eng_setrefreshdelay(ctx->_eng, delay);
            usleep(delay * 1000);
        }
    }

    LL_FOREACH(pos_node, ctx->_tailPos)
    {
        coordnode_h _npos = CAST(coordnode_h, pos_node);
        aoc_2d_eng_draw_sym_at(ctx->_eng, &_npos->_coord, "#", BGREEN);
    }

    ctx->_result = dll_size(&ctx->_tailPos);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ctx->_result);
    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CAST(context_h, _blk->_data);
    engine_free(ctx->_eng);
    dll_free_all(&ctx->_movs, free);
    ctx->_result = dll_size(&ctx->_tailPos);
    dll_free_all(&ctx->_tailPos, free);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;