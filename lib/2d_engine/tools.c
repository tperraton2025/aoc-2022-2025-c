#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <stdarg.h>
#include "aoc_types.h"

void eng_set_refresh_delay(aoc_2d_eng_h _eng, size_t delay)
{
    _eng->_delay = delay;
}

int aoc_2d_eng_draw_box(struct ascii_2d_engine *_eng)
{
    int ret = 0;

    coord_t _topLeft = {._x = _eng->_drawlimits._min._x, ._y = _eng->_drawlimits._min._y};
    coord_t _topRight = {._x = _eng->_drawlimits._max._x, ._y = _eng->_drawlimits._min._y};
    coord_t _botRight = {._x = _eng->_drawlimits._max._x, ._y = _eng->_drawlimits._max._y};
    coord_t _botLeft = {._x = _eng->_drawlimits._min._x, ._y = _eng->_drawlimits._max._y};

    engine_fill_hv_line(_eng, &_topLeft, &_topRight, AOC_2D_DIR_RIGHT, "═");
    engine_fill_hv_line(_eng, &_topRight, &_botRight, AOC_2D_DIR_DOWN, "║");
    engine_fill_hv_line(_eng, &_botRight, &_botLeft, AOC_2D_DIR_LEFT, "═");
    engine_fill_hv_line(_eng, &_botLeft, &_topLeft, AOC_2D_DIR_UP, "║");

    for (size_t _yy = _eng->_coordlimits._min._y; _yy <= _eng->_coordlimits._max._y; _yy++)
    {
        printf(MCUR_FMT, _eng->_drawlimits._min._y + _yy + 1, 1LU);
        printf("%*lu", 3, _yy);
    }

    for (size_t _xx = _eng->_coordlimits._min._x; _xx <= _eng->_coordlimits._max._x; _xx++)
    {
        if (_xx >= 100)
        {
            printf(MCUR_FMT, _eng->_drawlimits._min._y - 3, _eng->_drawlimits._min._x + _xx + 1);
            printf("%*lu", 1, (_xx / 100) % 10);
        }
        if (_xx >= 10)
        {
            printf(MCUR_FMT, _eng->_drawlimits._min._y - 2, _eng->_drawlimits._min._x + _xx + 1);
            printf("%*lu", 1, (_xx / 10) % 10);
        }
        printf(MCUR_FMT, _eng->_drawlimits._min._y - 1, _eng->_drawlimits._min._x + _xx + 1);
        printf("%*lu", 1, _xx % 10);
    }

    ret = aoc_2d_eng_draw_symbol_at(_eng, &_topLeft, "╔");
    if (ret)
        goto error;
    ret = aoc_2d_eng_draw_symbol_at(_eng, &_topRight, "╗");
    if (ret)
        goto error;
    ret = aoc_2d_eng_draw_symbol_at(_eng, &_botRight, "╝");
    if (ret)
        goto error;
    ret = aoc_2d_eng_draw_symbol_at(_eng, &_botLeft, "╚");
    if (ret)
        goto error;

    put_pos(_eng, &_eng->_cursor, &_topLeft);
error:
    return ret;
}

int engine_fill_drawing_area(struct ascii_2d_engine *_eng)
{
    if (_eng->_enabledraw)
    {
        for (size_t _line = _eng->_coordlimits._min._y; _line <= _eng->_coordlimits._max._y; _line++)
        {
            for (size_t _col = _eng->_coordlimits._min._x; _col <= _eng->_coordlimits._max._x; _col++)
            {
                coord_t _coord = {._x = _col, ._y = _line};
                aoc_2d_eng_draw_part_at(_eng, &_coord, &_eng->_voidsym);
            }
        }
    }
}

int engine_fill_hv_line(struct ascii_2d_engine *_eng, coord_t *_start, coord_t *_end, AOC_2D_DIR _dir, const char *_c)
{
    if (_eng->_enabledraw)
    {
        if (!_eng || !_start || _dir >= AOC_2D_DIR_MAX)
            return EINVAL;
        aoc_2d_eng_draw_symbol_at(_eng, _start, _c);
        put_pos(_eng, &_eng->_cursor, _start);
        do
            aoc_2d_eng_draw_symbol_at(_eng, &_eng->_cursor, _c);

        while (!move_cursor_until(_eng, _dir, 1, _end));
        engine_cursor_exit_drawing_area(_eng);
    }
    return 0;
}

void aoc_2d_eng_prompt_obj_list(aoc_2d_eng_h _eng)
{
    if (_eng->_enabledraw)
    {
        dll_sort(&_eng->_objects, pickhighestcoordinates);
        engine_cursor_user_stats(_eng);

        LL_FOREACH(_node, _eng->_objects)
        {
            if (_node->_obsolete)
            {
                struct object *_obj = CAST(struct object *, _node);
                dll_node_disconnect(&_eng->_objects, _node);
                aoc_2d_eng_free_obj(_obj);
            }
        }

        LL_FOREACH_EXT(_node, _eng->_objects)
        {
            if (_node->_obsolete)
                continue;
            size_t _maxbytes = 0;
            struct object *_obj = CAST(struct object *, _node);
            _maxbytes += printf("%-4s ", _obj->_name);
            _maxbytes += printf("[%*ld:", 4, _obj->_pos._x);
            _maxbytes += printf("%*ld]      ", 4, _obj->_pos._y);

            _eng->_statColOffset = _eng->_statColOffset > _maxbytes ? _eng->_statColOffset : _maxbytes;
            engine_cursor_user_next_stats(_eng);
        }
        engine_put_cursor_in_footer_area(_eng);
    }
}

void aoc_2d_eng_erase_stats(aoc_2d_eng_h _eng)
{
    if (_eng->_enabledraw)
    {
        engine_cursor_stats(_eng);
        printf(MCUR_FMT ERASE_LINE_FROM_CR, 0LU, _eng->_drawlimits._max._x + 1);
        for (size_t _lines = 0; _lines < _eng->_drawlimits._max._y + 1; _lines++)
        {
            printf(MCUR_FMT ERASE_LINE_FROM_CR, _lines, _eng->_drawlimits._max._x + 1);
        }
    }
}

void aoc_2d_eng_prompt_stats(aoc_2d_eng_h _eng)
{
    if (_eng->_enabledraw)
    {
        engine_cursor_stats(_eng);
        aoc_info("objects %ld", _eng->_objects._size);
        engine_cursor_private_next_stats(_eng);
        aoc_info("box size %s", strpos(&_eng->_coordlimits._max));
        engine_cursor_private_next_stats(_eng);
    }
}

void aoc_2d_eng_prompt(aoc_2d_eng_h _eng, const size_t _sleep, size_t _count, ...)
{
    if (_eng->_enabledraw)
    {
        engine_put_cursor_in_footer_area(_eng);
        printf(ERASE_LINE_FROM_CR);

        va_list _ap;
        va_start(_ap, _count);
        for (size_t _ii = 0; _ii < _count; _ii++)
        {
            char *_extra_str = va_arg(_ap, char *);
            printf("%s ", _extra_str);
        }
        va_end(_ap);

        if (_sleep)
            usleep(_sleep * 1000);
        printf("\n");
    }
}

bool coord_compare(void *_a, void *_b)
{
    coord_tracker_h _aNode = CAST(coord_tracker_h, _a);
    coord_tracker_h _bNode = CAST(coord_tracker_h, _b);
    return (_aNode->_coord._x == _bNode->_coord._x) && (_aNode->_coord._y == _bNode->_coord._y);
}