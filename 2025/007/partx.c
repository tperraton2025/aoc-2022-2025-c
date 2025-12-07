#include "partx.h"

int objectsparser(void *arg, char *str)
{
    context_h _ctx = (context_h)arg;
    _ctx->_cursor._x = 0;

    char *_symlist[] = {"^", "S"};

    ARR_FOREACH(_it, _symlist)
    {
        _ctx->_cursor._y;
        char *_srcsym = _symlist[_it];
        char *_start = str;
        char *_strpi = str;

        _strpi = strchr(str, *_srcsym);
        while (_strpi)
        {
            _ctx->_cursor._x = _strpi - _start;
            aoc_2d_obj_h _nobj = aoc_2d_obj_ctor(_ctx->_eng, _srcsym, &_ctx->_cursor, _srcsym, OBJ_PROPERTY_STATIC);
            aoc_2d_eng_append_obj(_ctx->_eng, _nobj);
            _strpi = strchr(_strpi + 1, *_srcsym);
        }
        _ctx->_cursor._y++;
    }
    return 0;
}

parser_t _objparser = {._name = "objectsparser", ._func = objectsparser};

int propagate(context_h ctx)
{
    aoc_2d_obj_h start = aoc_2d_eng_get_obj_by_name(ctx->_eng, "S");
    if (!start)
        return EINVAL;

    ctx->_cursor = aoc_2d_eng_get_obj_position(ctx->_eng, start);
}