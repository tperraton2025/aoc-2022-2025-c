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
        size_t _limit = 0;
        coordnode_h _trkh = (coordnode_h)_posn;
        coord_t *_posh = &_trkh->_coord;

        _limit += dll_try_count_nodes_by_property(_posn, (void *)_posn, coord_dist_less_than_2, coord_dist_more_than_2);
        if (4 > _limit)
            _posn->_obsolete = true;
    }
    LL_FOREACH_P_EXT(_posn, allpos)
    {
        if (_posn->_obsolete)
        {
            liberated++;
            coordnode_h _trkh = (coordnode_h)_posn;
            coord_t *_posh = &_trkh->_coord;
            aoc_2d_eng_draw_part_at(eng, _posh, "x", RED);
            ghostnode._next = _posn->_next;
            dll_node_disconnect(allpos, _posn);
            free(_posn);
            if (!ghostnode._next)
                break;
            _posn = &ghostnode;
        }
    }
    return liberated;
}
