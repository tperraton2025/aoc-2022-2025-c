#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    aoc_2d_eng_h _eng;
    dll_head_h _rollspositions;
    struct dll_head _parsers;
    size_t _result;
};

static int parserolls(void *arg, char *str)
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
        aoc_2d_obj_h _objh = aoc_2d_obj_ctor(eng, _objname, &_pos, "@", OBJ_PROPERTY_STATIC, GREEN);
        if (!_objh)
            _ret = ENOMEM;
        if (!_ret)
            _ret = aoc_2d_eng_append_obj(eng, _objh);
        _start = strchr(_start, '@');
    }
    engine_add_line(eng);
    return _ret;
}

static struct parser blockparser = {._name = "roll parsers", ._func = parserolls};


static size_t trimaccessiblepositions(aoc_2d_eng_h eng, dll_head_h allpos)
{
    size_t liberated = 0;
    size_t startlen = allpos->_size;
    LL_FOREACH_P(_posn, allpos)
    {
        size_t _limit = 0;
        coord_tracker_h _trkh = (coord_tracker_h)_posn;
        coord_t *_posh = &_trkh->_coord;

        _limit += dll_try_count_nodes_by_property(_posn, (void *)_posn, coord_dist_less_than_2, coord_dist_more_than_2);
        if (4 > _limit)
            _posn->_obsolete = true;
    }
    dll_node_t _dummy = {0};
    LL_FOREACH_P_EXT(_posn, allpos)
    {
        if (_posn->_obsolete)
        {
            liberated++;
            coord_tracker_h _trkh = (coord_tracker_h)_posn;
            coord_t *_posh = &_trkh->_coord;
            aoc_2d_eng_draw_part_at(eng, _posh, "x", RED);
            _dummy._next = _posn->_next;
            dll_node_disconnect(allpos, _posn);
            free(_posn);
            if (!_dummy._next)
                break;
            _posn = &_dummy;
        }
    }
    return liberated;
}
typedef struct context *aoc_context_h;
static void markfreerolls(struct context *_ctx);