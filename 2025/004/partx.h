#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

static int parserolls(void *arg, char *str)
{
    int _ret = 0;
    aoc_2d_engine_h _eng = (aoc_2d_engine_h)arg;

    char *_start = str;
    _start = strchr(_start, '@');
    while (_start && !_ret)
    {
        coord_t _pos = {._x = _start - str, ._y = engine_last_line(_eng)};
        if (_start)
            _start++;

        char _objname[] = "roll9999999";
        sprintf(_objname, "roll%-3s", strobjcnt(_eng));
        aoc_2d_object_h _objh = aoc_engine_object(_eng, _objname, &_pos, "@", OBJ_PROPERTY_STATIC);
        if (!_objh)
            _ret = ENOMEM;
        if (!_ret)
            _ret = aoc_engine_append_obj(_eng, _objh);
        _start = strchr(_start, '@');
    }
    engine_add_line(_eng);
    return _ret;
}

static struct parser blockparser = {._name = "roll parsers", ._func = parserolls};
