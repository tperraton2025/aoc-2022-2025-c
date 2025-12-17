#include "part1.h"

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_result = 0;

    _ctx->_pos._x = 1;
    _ctx->_pos._y = 1;

    _ctx->_grippedBox = NULL;

    dll_head_init(&_ctx->_cmds);
    dll_head_init(&_ctx->_columns);
    dll_head_init(&_ctx->_parsers);

    parser_append(&_ctx->_parsers, &blockparser, _ctx);
    parser_append(&_ctx->_parsers, &commandparser, &_ctx->_cmds);

    coord_t _prelcoordmaxima = {._x = 2, ._y = 2};

    _ctx->_eng = aoc_2d_eng_create(&_prelcoordmaxima, '~', 0, bycoordinatesYfirst, false);

    if (!_ctx->_eng)
        goto cleanup;

    aoc_2d_eng_parse_cli(&_ctx->_eng, argc, argv);
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
    char test[16] = {0};

    aoc_2d_eng_extend_one_direction(_ctx->_eng, 20, AOC_2D_DIR_UP);
    aoc_2d_eng_draw(_ctx->_eng);

    crane_action(_ctx);
    aoc_spell_ans(_blk);

    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);

    engine_free(_ctx->_eng);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;

static int crate_lift(struct context *_ctx, command_t *_cmd)
{
    coordpair_t boundaries = aoc_2d_eng_get_parts_boundaries(_ctx->_eng);
    for (size_t _ii = 0; _ii <= boundaries._max._y; _ii++)
    {
        coord_t _gripper = {._x = _cmd->from, ._y = _ii};
        _ctx->_grippedBox = aoc_2d_eng_get_obj_by_position(_ctx->_eng, &_gripper);
        if (_ctx->_grippedBox)
        {
            aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "gripped", aoc_2d_obj_name(_ctx->_grippedBox));
            while (!aoc_2d_eng_step_obj(_ctx->_eng, _ctx->_grippedBox, 1LU, AOC_2D_DIR_UP, GREEN))
                aoc_2d_eng_prompt_obj_list(_ctx->_eng);
            return 0;
        }
    }
    char _coord_str[4] = "";
    sprintf(_coord_str, "%3ld", _cmd->from);
    aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "nothing to grip at", _coord_str);
    return 1;
}

static int crate_change_lane(struct context *_ctx, command_t *_cmd)
{
    int ret = 0;
    if (!_ctx->_grippedBox)
        aoc_err("%s", "NULL pointer gripped box");
    size_t _ii = 0;
    for (; (_ii < 100) && (_ctx->_pos._x != _cmd->to); _ii++)
    {
        AOC_2D_DIR dir = _ctx->_pos._x > _cmd->to ? AOC_2D_DIR_LEFT : AOC_2D_DIR_RIGHT;
        _ctx->_pos._x = _ctx->_pos._x > _cmd->to ? _ctx->_pos._x - 1 : _ctx->_pos._x + 1;

        if (aoc_2d_eng_step_obj(_ctx->_eng, _ctx->_grippedBox, 1LU, dir, GREEN))
        {
            aoc_2d_eng_prompt_extra_stats_as_err(_ctx->_eng, "moving %s failed", aoc_2d_obj_name(_ctx->_grippedBox));
        }
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);

        char _dest_col_str[4] = "";
        sprintf(_dest_col_str, "%3ld", _cmd->to);
        aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "at lane %ld", _dest_col_str);
    }
    assert(_ii != 100 && "%s failed");
    return 0;
}

static int crate_deposit(struct context *_ctx, command_t *_cmd)
{
    while (!aoc_2d_eng_step_obj(_ctx->_eng, _ctx->_grippedBox, 1LU, AOC_2D_DIR_DOWN, GREEN))
    {
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);
        aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "depositing", aoc_2d_obj_name(_ctx->_grippedBox));
    }
    aoc_2d_eng_draw_obj(_ctx->_eng, _ctx->_grippedBox, NULL);
    return 0;
}

static int crane_action(struct context *_ctx)
{
    size_t inc = 0;
    char _sCnt[4] = "";
    char _SFrom[4] = "";
    char _sDest[4] = "";

    LL_FOREACH(_node, _ctx->_cmds)
    {
        inc++;
        command_t *_cmd = CAST(command_t *, _node);
        sprintf(_sCnt, "%3ld", _cmd->count);
        sprintf(_SFrom, "%3ld", _cmd->from);
        sprintf(_sDest, "%3ld", _cmd->to);
        aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "move", _sCnt, "from", _SFrom, "to", _sDest);

        for (size_t _repeat = 0; _repeat < _cmd->count; _repeat++)
        {
            if (crate_lift(_ctx, _cmd))
                continue;
            if (crate_change_lane(_ctx, _cmd))
                continue;
            if (crate_deposit(_ctx, _cmd))
                continue;
        }
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);
    }
    dll_free_all(&_ctx->_cmds, free);
    dll_free_all(&_ctx->_columns, free);

    char _uCmdCnt[4] = "";
    sprintf(_uCmdCnt, "%3ld", inc);
    aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, _uCmdCnt, "program finished commands executed");
    return 0;
}

static void aoc_spell_ans(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    size_t _colCount = _ctx->_columns._size + 1;
    _ctx->spelling = malloc(_colCount);
    char *_p = _ctx->spelling;
    memset(_ctx->spelling, '\0', _colCount);
    for (size_t _col = 0; _col < _colCount; _col++)
    {
        command_t _spell1 = {.count = 1, .from = _col * 4, .to = _col * 4};
        crate_lift(_ctx, &_spell1);
        if (!_ctx->_grippedBox)
            continue;
        _p += sprintf(_p, "%c", aoc_2d_obj_name(_ctx->_grippedBox)[1]);
        crate_deposit(_ctx, &_spell1);
    }
    aoc_ans("AOC 2022 %s solution is %s", _blk->_name, _ctx->spelling);

    FREE(_ctx->spelling);
}

static int parsecommand(void *arg, char *_str)
{
    dll_head_h _cmds = (dll_head_h)arg;
    int match = 0;
    int ret = 0;
    command_t _nc = {0};
    match = sscanf(_str, "move %ld from %ld to %ld", &_nc.count, &_nc.from, &_nc.to);
    if (3 == match)
    {
        _nc.from *= 4;
        _nc.to *= 4;
        command_t *_pnc = command_ctor(&_nc);
        if (!_pnc)
            return ENOMEM;
        ret = dll_node_append(_cmds, CAST(dll_node_h, _pnc));
        if (ret)
        {
            FREE(_pnc);
            return ENOMEM;
        }
        return 0;
    }
    return -1;
}

static int parseblock(void *arg, char *_str)
{
    aoc_context_h _ctx = (aoc_context_h)arg;
    int match = 0;
    int ret = 0;
    size_t _len = strnlen(_str, MAX_LINE_LEN);
    if (_len <= 3)
        return -1;
    _ctx->_pos._x = 1;

    char _buf[] = "[A]";

    for (char *_ss = strchr(_str, '['); _ss; _ss = strchr(_ss + 1, '['))
    {
        _ctx->_pos._x = (_ss - _str);
        memcpy(_buf, _ss, 3);
        match += sscanf(_buf, "[%c]", &_buf[1]);
        if (!match)
            continue;
        if (N_BETWEEN_AB(_buf[1], 'A', 'Z'))
        {
            aoc_2d_obj_h _nobj = aoc_2d_obj_ctor(_ctx->_eng, _buf, &_ctx->_pos, _buf, OBJ_FLAG_MOBILE, "");
            ret = aoc_2d_eng_append_obj(_ctx->_eng, _nobj);
            if (ret)
                break;
        }
        if (!dll_node_find_by_property(&_ctx->_columns, &_ctx->_pos._x, coord_same_column))
        {
            coord_tracker_h _trck = coordtrackernode_ctor();
            _trck->_coord._x = _ctx->_pos._x;
            dll_node_sorted_insert(&_ctx->_columns, NODE_CAST(_trck), highest_column);
        }
        _ctx->_pos._x++;
    }
    _ctx->_pos._y++;
    return match ? 0 : -1;
}
