#include "partx.h"

typedef struct context
{
    aoc_2d_eng_h _eng;
    aoc_2d_obj_h _head;
    struct dll_head _tails;
    struct dll_head _movs;
    struct dll_head _tailPos;
    aoc_2d_obj_h _lastMovedLink;
    coord_t _prevPosFromLastMovedLink;
    size_t _result;
} context_t;
typedef context_t *context_h;

#define CTX_CAST(_p) ((struct context *)_p)

static int track_tail(context_h ctx, coord_t *_pos)
{
    coordnode_t _posTrack = {._coord = {._x = _pos->_x, _pos->_y}};
    if (NULL == dll_node_find_by_property(&ctx->_tailPos, (void *)&_posTrack, coordnodes_equal))
    {
        coordnode_h _npos = malloc(sizeof(coordnode_t));
        if (!_npos)
            return ENOMEM;
        _npos->_coord._x = _pos->_x;
        _npos->_coord._y = _pos->_y;
        int ret = dll_node_append(&ctx->_tailPos, NODE_CAST(_npos));
        if (ret)
            FREE(_npos);
        return ret;
    }
    return EALREADY;
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    int ret = 0;
    _blk->_data = malloc(sizeof(context_t));
    if (!_blk->_data)
        return ENOMEM;
    context_h ctx = CTX_CAST(_blk->_data);

    ctx->_result = 0;

    coord_t _prelcoordmaxima = {._x = 20, ._y = 20};

    ctx->_head = NULL;

    dll_head_init(&ctx->_movs);
    dll_head_init(&ctx->_tailPos);
    dll_head_init(&ctx->_tails);

    ctx->_eng = aoc_2d_eng_create(&_prelcoordmaxima, '~', 0, NULL, false);

    if (!ctx->_eng)
    {
        ret = ENOMEM;
        goto error;
    }

    aoc_2d_eng_parse_cli(ctx->_eng, argc, argv);

    coord_t start = {0};
    aoc_2d_eng_get_canvas_center(ctx->_eng, &start);

    aoc_2d_eng_extend_one_direction(ctx->_eng, 50, AOC_2D_DIR_UP);
    aoc_2d_eng_extend_one_direction(ctx->_eng, 50, AOC_2D_DIR_RIGHT);

    ctx->_head = aoc_2d_obj_ctor(ctx->_eng, "head", &start, "H", OBJ_FLAG_NO_COLLISION | OBJ_FLAG_MOBILE, BGREEN);

    if (!ctx->_head)
    {
        ret = ENOMEM;
        goto error;
    }

    ret = aoc_2d_eng_append_obj(ctx->_eng, ctx->_head);
    if (ret)
        goto error;

    for (size_t ii = 1; ii < 10; ii++)
    {
        char name[] = "1";
        sprintf(name, "%1ld", ii);
        char fullname[] = "tail %1ld";
        sprintf(fullname, "tail %1ld", ii);
        aoc_2d_obj_h ntail = aoc_2d_obj_ctor(ctx->_eng, fullname, &start, name, OBJ_FLAG_NO_COLLISION | OBJ_FLAG_NO_COLLISION, "");

        if (!ntail)
            ret = ENOMEM;
        if (ret)
            goto error;
        else
            ret = aoc_2d_eng_append_obj(ctx->_eng, ntail);
        if (ret)
            goto error;
        dll_node_append(&ctx->_tails, NODE_CAST(ntail));
    }
    track_tail(ctx, &start);

    aoc_2d_eng_draw(ctx->_eng);
    aoc_2d_eng_prompt_obj_list(ctx->_eng);
    return 0;

error:
    if (ctx->_eng)
        engine_free(ctx->_eng);
    else
    {
        if (ctx->_head)
            FREE(ctx->_head);

        dll_free_all(&ctx->_movs, free);
        dll_free_all(&ctx->_tailPos, free);
        dll_free_all(&ctx->_tails, aoc_2d_eng_free_obj);
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
    struct context *ctx = CTX_CAST(_blk->_data);

    if (ctx->_head == _head)
    {
        aoc_info(MCUR_FMT ERASE_LINE_FROM_CR BRED "moved head : %s - %lu\n" RESET, 34LU, 100LU, AOC_2D_DIR_TXT[_dir], 1LU);
        if (aoc_2d_eng_step_obj(ctx->_eng, _head, _dir, 1LU, NULL) == ERANGE)
            aoc_2d_eng_prompt_extra_stats_as_err(ctx->_eng, "error while moving %s", aoc_2d_obj_name(_head));
    }
    if (1 < aoc_2d_eng_get_dist_between_objects(ctx->_eng, _head, _tail))
    {
        if (aoc_2d_eng_move_one_step_towards(ctx->_eng, _tail, *aoc_2d_obj_get_pos(_head)) == ERANGE)
            aoc_2d_eng_prompt_extra_stats_as_err(ctx->_eng, "error while moving %s", aoc_2d_obj_name(_tail));
    }

    aoc_info(MCUR_FMT ERASE_LINE_FROM_CR BRED "last moved: %s\n" RESET, 33LU, 100LU, aoc_2d_obj_name(_tail));
    usleep(100 * 1000);
    ctx->_lastMovedLink = _tail;

    if (0 == strcmp(aoc_2d_obj_name(_tail), "tail 9"))
    {
        coord_t _prevTailPosition = *aoc_2d_obj_get_pos(_tail);
        track_tail(ctx, &_prevTailPosition);
    }
    aoc_2d_eng_prompt_obj_list(ctx->_eng);
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    movement_t *nmov = NULL;
    context_h ctx = CTX_CAST(_blk->_data);
    size_t delay = aoc_2d_eng_getrefreshdelay(ctx->_eng);
    LL_FOREACH(_node, ctx->_movs)
    {
        nmov = MOV_CAST(_node);
        aoc_info(MCUR_FMT ERASE_LINE_FROM_CR BRED "current move : %s - %lu\n" RESET, 32LU, 100LU, AOC_2D_DIR_TXT[nmov->dir], nmov->steps);

        coordnode_h _npos = NULL;
        for (size_t ii = 0; ii < nmov->steps; ii++)
        {
            ctx->_lastMovedLink = ctx->_head;
            aoc_2d_eng_setrefreshdelay(ctx->_eng, 0LU);
            LL_FOREACH(_node, ctx->_tails)
            {
                aoc_2d_obj_h _tail = CAST(aoc_2d_obj_h, _node);
                int_refresh_link(_blk, ctx->_lastMovedLink, _tail, nmov->dir);
            }
            aoc_2d_eng_setrefreshdelay(ctx->_eng, delay);
            usleep(delay * 1000);
        }
    }

    LL_FOREACH(pos_node, ctx->_tailPos)
    {
        coordnode_h _npos = CAST(coordnode_h, pos_node);
        aoc_2d_eng_draw_sym_at(ctx->_eng, &_npos->_coord, "#", "");
    }

    ctx->_result = dll_size(&ctx->_tailPos);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ctx->_result);

    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    engine_free(_ctx->_eng);
    dll_free_all(&_ctx->_tailPos, free);
    dll_free_all(&_ctx->_movs, free);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part2 = &privPart2;