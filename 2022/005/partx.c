#include "partx.h"

typenode_ctor(column, new->_col_index = col_index; new->_col_x = col_x;, size_t col_x, size_t col_index);
void printstacks(dll_head_h columns);

static char _buf[] = "[A]";

int parseblock(void *arg, char *_str)
{
    aoc_context_h _ctx = (aoc_context_h)arg;
    int match = 0;
    int ret = 0;
    size_t _len = strnlen(_str, MAX_LINE_LEN);
    if (_len <= 3)
        return -1;
    _ctx->_pos._x = 1;

    size_t columnindex = 1;

    for (char *_ss = strchr(_str, '['); _ss; _ss = strchr(_ss + 1, '['))
    {
        _ctx->_pos._x = (_ss - _str);
        memcpy(_buf, _ss, 3);
        match += sscanf(_buf, "[%c]", &_buf[1]);
        if (!match)
            continue;
        aoc_2d_obj_h _nobj = NULL;
        if (N_BETWEEN_AB(_buf[1], 'A', 'Z'))
        {
            columnindex = 1LU + ((_ctx->_pos._x) / sizeof("[A]"));

            coord_t objcenter = _ctx->_pos;
            objcenter._x++;

            _nobj = aoc_2d_obj_ctor(_ctx->_eng, _buf, &_ctx->_pos, _buf, OBJ_FLAG_MOBILE, "");
            assert(_nobj);
            assert(!aoc_2d_eng_append_obj(_ctx->_eng, _nobj));
            columnnode_h boxstack = (columnnode_h)dll_node_find_by_property(&_ctx->_columns, &objcenter._x, column_hassame_x);
            if (!boxstack)
            {
                boxstack = (columnnode_h)columnnode_ctor(objcenter._x, columnindex);
                dll_node_sorted_insert(&_ctx->_columns, &boxstack->_node, column_haslower_x);
            }
            if (_ctx->_verbose)
                aoc_info("-- new block %s at %s, fitting column %lu (%lu)", _buf, strpos(&objcenter), boxstack->_col_index, boxstack->_col_x);
            dll_node_sorted_insert(&boxstack->_boxes, &aoc_2d_obj_ref_ctor(_nobj)->_node, posnode_ydecr_xdecr);
        }
        _ctx->_pos._x++;
    }
    _ctx->_pos._y++;
    return match ? 0 : -1;
}

int parsecommand(void *arg, char *_str)
{
    dll_head_h _cmds = (dll_head_h)arg;
    int match = 0;
    int ret = 0;
    command_t _nc = {0};
    match = sscanf(_str, "move %lu from %lu to %lu", &_nc.count, &_nc.from, &_nc.to);
    if (3 == match)
    {
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

bool column_hassame_x(void *arga, void *argb)
{
    columnnode_h _colh = (columnnode_h)arga;
    return _colh->_col_x == *(size_t *)argb;
}

bool column_hassame_index(void *arga, void *argb)
{
    columnnode_h _colh = (columnnode_h)arga;
    return _colh->_col_index == *(size_t *)argb;
}

dll_node_h column_haslower_x(dll_node_h arga, dll_node_h argb)
{
    columnnode_h _cola_h = (columnnode_h)arga;
    columnnode_h _colb_h = (columnnode_h)argb;
    return _cola_h->_col_x < _colb_h->_col_x ? arga : argb;
}

static char _coord_str[4] = "";

int crate_lift(struct context *_ctx, command_t *_cmd, size_t boxestolift)
{
    coordpair_t boundaries = aoc_2d_eng_get_parts_boundaries(_ctx->_eng);

    columnnode_h boxstack = (columnnode_h)dll_node_find_by_property(&_ctx->_columns, &_cmd->from, column_hassame_index);

    assert(boxstack);
    assert(boxstack->_boxes._size);

    aoc_2d_obj_ref_h topick = NULL;

    if (_ctx->_verbose)
        printf(ERASE_ALL_FROM_CURSOR "-- lifting ");
    RANGE_FOR(ind, 0, boxestolift)
    {
        topick = (aoc_2d_obj_ref_h)boxstack->_boxes._last;
        assert(topick);
        dll_node_append(&_ctx->_grippedBoxes, &aoc_2d_obj_ref_ctor(topick->data)->_node);
        dll_node_disconnect(&boxstack->_boxes, &topick->_node);
        if (_ctx->_verbose)
            printf(" %s", aoc_2d_obj_name(topick->data));
        FREE(topick);
    }

    if (_ctx->_verbose)
    {
        printf("\n");
        printstacks(&_ctx->_columns);
    }

    if (!_ctx->_grippedBoxes._size)
        goto nothingtogrip;

    bool alllifted = false;
    size_t delay = aoc_2d_eng_getrefreshdelay(_ctx->_eng);
    aoc_2d_eng_setrefreshdelay(_ctx->_eng, 0LU);

    while (!alllifted)
    {
        alllifted = true;

        LL_FOREACH(boxn, _ctx->_grippedBoxes)
        {
            if (!aoc_2d_eng_step_obj(_ctx->_eng, ((aoc_2d_obj_ref_h)boxn)->data, 1LU, AOC_2D_DIR_UP, BGREEN))
                alllifted = false;
        }
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);
        if (_ctx->_verbose)
            usleep(delay * 1000);
    }

    aoc_2d_eng_setrefreshdelay(_ctx->_eng, delay);
    return 0;

nothingtogrip:
    sprintf(_coord_str, "%3ld", _cmd->from);
    aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "nothing to grip at", _coord_str);
    return 1;
}

static char _dest_col_str[4] = "";

int crate_change_lane(struct context *_ctx, command_t *_cmd)
{
    int ret = 0;
    if (!_ctx->_grippedBoxes._size)
        aoc_err("%s", "no gripped box to transfer");
    size_t _ii = 0;
    columnnode_h boxstack = (columnnode_h)dll_node_find_by_property(&_ctx->_columns, &_cmd->to, column_hassame_index);
    assert(boxstack);
    _ctx->_pos = *aoc_2d_obj_get_pos(((aoc_2d_obj_ref_h)_ctx->_grippedBoxes._first)->data);
    size_t delay = aoc_2d_eng_getrefreshdelay(_ctx->_eng);
    aoc_2d_eng_setrefreshdelay(_ctx->_eng, 0LU);
    for (; (_ii < 100) && (_ctx->_pos._x != boxstack->_col_x); _ii++)
    {

        LL_FOREACH(boxn, _ctx->_grippedBoxes)
        {
            _ctx->_pos = *aoc_2d_obj_get_pos(((aoc_2d_obj_ref_h)boxn)->data);
            AOC_2D_DIR dir = _ctx->_pos._x > boxstack->_col_x ? AOC_2D_DIR_LEFT : AOC_2D_DIR_RIGHT;

            if (aoc_2d_eng_step_obj(_ctx->_eng, ((aoc_2d_obj_ref_h)boxn)->data, 1LU, dir, BGREEN))
            {
                aoc_2d_eng_prompt_extra_stats_as_err(_ctx->_eng, "moving %s to %lu failed", aoc_2d_obj_name(((aoc_2d_obj_ref_h)boxn)->data), boxstack->_col_x);
            }
        }
        LL_FOREACH_EXT(boxn, _ctx->_grippedBoxes)
        {
            snprintf(_dest_col_str, sizeof(_dest_col_str), "%3ld", _cmd->to);
            _ctx->_pos = *aoc_2d_obj_get_pos(((aoc_2d_obj_ref_h)boxn)->data);
        }
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);

        if (_ctx->_verbose)
            usleep(delay * 1000);
    }
    aoc_2d_eng_setrefreshdelay(_ctx->_eng, delay);
    assert(_ii != 100 && "%s failed");
    return 0;
}

int crate_deposit(struct context *_ctx, command_t *_cmd, size_t boxestodeposit)
{
    bool alldeposited = false;
    columnnode_h boxstack = (columnnode_h)dll_node_find_by_property(&_ctx->_columns, &_cmd->to, column_hassame_index);

    size_t delay = aoc_2d_eng_getrefreshdelay(_ctx->_eng);
    aoc_2d_eng_setrefreshdelay(_ctx->_eng, 0LU);
    while (!alldeposited)
    {
        alldeposited = true;
        LL_FOREACH_REV(boxn, _ctx->_grippedBoxes)
        {
            if (!aoc_2d_eng_step_obj(_ctx->_eng, ((aoc_2d_obj_ref_h)boxn)->data, 1LU, AOC_2D_DIR_DOWN, BGREEN))
                alldeposited = false;
        }
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);

        if (_ctx->_verbose)
            usleep(delay * 1000);
    }

    RANGE_FOR(ind, 0, boxestodeposit)
    {
        aoc_2d_eng_draw_obj(_ctx->_eng, ((aoc_2d_obj_ref_h)_ctx->_grippedBoxes._last)->data, "");
        dll_node_append(&boxstack->_boxes, &aoc_2d_obj_ref_ctor(((aoc_2d_obj_ref_h)_ctx->_grippedBoxes._last)->data)->_node);
        dll_node_h todelete = _ctx->_grippedBoxes._last;
        dll_node_disconnect(&_ctx->_grippedBoxes, todelete);
        FREE(todelete);
    }

    if (_ctx->_verbose)
        printstacks(&_ctx->_columns);

    aoc_2d_eng_setrefreshdelay(_ctx->_eng, delay);
    dll_free_all(&_ctx->_grippedBoxes, free);
    return 0;
}

void aoc_spell_ans(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);

    _ctx->spelling = calloc(_ctx->_columns._size, sizeof("[A]") + 1);
    char *_p = _ctx->spelling;

    LL_FOREACH(column, _ctx->_columns)
    {
        columnnode_h colcast = (columnnode_h)column;
        _p += sprintf(_p, "%c", aoc_2d_obj_name(((aoc_2d_obj_ref_h)(colcast->_boxes._last))->data)[1]);
    }
    if (_ctx->_verbose)
        printstacks(&_ctx->_columns);
    aoc_ans("AOC 2022 %s solution is %s", _blk->_name, _ctx->spelling);

    FREE(_ctx->spelling);
}

void printstacks(dll_head_h columns)
{
    LL_FOREACH_P(colnode, columns)
    {
        columnnode_h colcast = (columnnode_h)colnode;
        printf(ERASE_ALL_FROM_CURSOR "-- boxes stacks %lu: x=%lu", colcast->_col_index, colcast->_col_x);
        if (colcast->_boxes._size)
        {
            LL_FOREACH(box, colcast->_boxes)
            {
                printf("%s", aoc_2d_obj_name(((aoc_2d_obj_ref_h)box)->data));
            }
            printf("\n");
        }
    }
}

static char scount[4] = "";
static char sfrom[4] = "";
static char sdest[4] = "";
static char cmdscount[4] = "";

int crane_action(struct context *_ctx, bool multiboxmode)
{
    size_t inc = 0;
    if (_ctx->_verbose)
        printstacks(&_ctx->_columns);
    LL_FOREACH(_node, _ctx->_cmds)
    {
        inc++;
        command_t *cmd = CAST(command_t *, _node);
        snprintf(scount, sizeof(scount), "%ld", cmd->count);
        snprintf(sfrom, sizeof(sfrom), "%ld", cmd->from);
        snprintf(sdest, sizeof(sdest), "%ld", cmd->to);
        aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, "move", scount, "from", sfrom, "to", sdest);

        for (size_t _repeat = 0; _repeat < (multiboxmode ? 1LU : cmd->count); _repeat++)
        {
            if (crate_lift(_ctx, cmd, multiboxmode ? cmd->count : 1LU))
                continue;
            if (crate_change_lane(_ctx, cmd))
                continue;
            if (crate_deposit(_ctx, cmd, multiboxmode ? cmd->count : 1LU))
                continue;
        }
        aoc_2d_eng_prompt_obj_list(_ctx->_eng);
    }
    dll_free_all(&_ctx->_cmds, free);

    sprintf(cmdscount, "%3ld", inc);
    aoc_2d_eng_prompt_multistr(_ctx->_eng, SLEEP_TIME_MS, cmdscount, "program finished commands executed");
    return 0;
}
