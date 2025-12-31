#include "partx.h"

struct parser blockparser = {._name = "roll parsers", ._func = parserolls};

int parserolls(void *arg, char *str)
{
    int _ret = 0;
    aoc_2d_eng_h eng = (aoc_2d_eng_h)arg;

    char *_start = str;
    _start = strchr(_start, '@');
    while (_start && !_ret)
    {
        coord_t _pos = {._x = _start - str, ._y = engine_last_line(eng)};
        if (_start)
            _start++;

        char _objname[] = "roll9999999";
        sprintf(_objname, "roll%-3s", strobjcnt(eng));
        aoc_2d_obj_h _objh = aoc_2d_obj_ctor(eng, _objname, &_pos, "@", OBJ_FLAG_STATIC, GREEN);
        if (!_objh)
            _ret = ENOMEM;
        if (!_ret)
            _ret = aoc_2d_eng_append_obj(eng, _objh);
        _start = strchr(_start, '@');
    }
    engine_add_line(eng);
    return _ret;
}

size_t trimaccessiblepositions(aoc_2d_eng_h eng, dll_head_h allpos)
{
    size_t liberated = 0;
    size_t startlen = allpos->_size;
    dll_node_t ghostnode = {0};
    LL_FOREACH_P(_posn, allpos)
    {
        size_t _obstacles = 0;
        coord_t *_posh = &((coordnode_h)_posn)->_coord;

        uint2D_t scanmin = {._xy._x = _posh->_x ? _posh->_x - 1 : _posh->_x,
                            ._xy._y = _posh->_y ? _posh->_y - 1 : _posh->_y};
        uint2D_t scanmax = {._xy._x = _posh->_x + 1, ._xy._y = _posh->_y + 1};
        uint2D_t scan = scanmin;

        do
        {
            coord_t _pos = {._x = scan._xy._x, ._y = scan._xy._y};
            if (_pos._x == _posh->_x && _pos._y == _posh->_y)
                continue;

            if (aoc_2d_eng_get_obj_by_position(eng, &_pos))
                _obstacles++;

        } while (!bscanuint2d(&scan, &scanmin, &scanmax));

        if (4 > _obstacles)
        {
            _posn->_obsolete = true;
        }
    }
    LL_FOREACH_P_EXT(_posn, allpos)
    {
        if (_posn->_obsolete)
        {
            liberated++;
            coord_t *_posh = &((coordnode_h)_posn)->_coord;
            aoc_2d_obj_delete(eng, aoc_2d_eng_get_obj_by_position(eng, _posh));
            aoc_2d_eng_draw_sym_at(eng, _posh, "x", RED);

            ghostnode._next = _posn->_next;
            dll_node_disconnect(allpos, _posn);
            free(_posn);
            _posn = &ghostnode;
            if (!ghostnode._next)
                break;
        }
    }
    return liberated;
}
