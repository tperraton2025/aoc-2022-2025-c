#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <fcntl.h>

void engine_free(aoc_2d_engine_h _eng)
{
    engine_cursor_exit_drawing_area(_eng);
    dll_free_all(&_eng->_objects, aoc_engine_free_object);
    FREE_AND_CLEAR_P(_eng);
}

aoc_2d_engine_h engine_create(coord_t partcoordmaxima, char _voidsym, size_t delay)
{
    aoc_2d_engine_h _ret = NULL;
    TRY_RAII_MALLOC(_ret, sizeof(struct ascii_2d_engine));

    if (!_ret)
        return NULL;
    coordboundaries_t _drawboundariesmax = {._max = {._x = ABSOLUTE_MAX_X, ._x = ABSOLUTE_MAX_Y},
                                            ._min = {._x = 1, ._y = 1}};

    dll_head_init(&_ret->_objects);

    _ret->_delay = delay;

    if (!COORD_RANGE_CHECK(partcoordmaxima, _drawboundariesmax))
    {
        _ret->_drawboundaries._min._x = 1;
        _ret->_drawboundaries._min._y = 1;

        _ret->_drawboundaries._max._x = partcoordmaxima._x + 3;
        _ret->_drawboundaries._max._y = partcoordmaxima._y + 3;

        _ret->_partboundaries._min._x = 2;
        _ret->_partboundaries._min._y = 2;

        _ret->_partboundaries._max._x = partcoordmaxima._x + 2;
        _ret->_partboundaries._max._y = partcoordmaxima._y + 2;

        _ret->_cursor._x = _ret->_drawboundaries._min._x;
        _ret->_cursor._y = _ret->_drawboundaries._min._y;

        _ret->_voidsym = _voidsym;

        _ret->_statLine = 2;
        _ret->_PrivStatLine = _ret->_statLine;

        _ret->_enabledraw = _ret->_drawboundaries._max._x < DRAWABLE_MAX_X && _ret->_drawboundaries._max._y < DRAWABLE_MAX_Y;
        if (!_ret->_enabledraw)
            aoc_warn("%s", "drawable zone will not fit in a terminal window \n\
                drawing is disabled until forced");
        return _ret;
    }
    else
        goto cleanup;

cleanup:
    FREE_AND_CLEAR_P(_ret);
    return NULL;
}

int engine_retract_drawing_area(struct ascii_2d_engine *_eng, move_t cellsdelta)
{
    if (!_eng)
        return EINVAL;
    if (cellsdelta._x >= _eng->_drawboundaries._min._x)
        return EINVAL;
    if (cellsdelta._y >= _eng->_drawboundaries._min._y)
        return EINVAL;

    coord_t _boundary_MAX = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y};
    coord_t _retracttest = {._x = _eng->_drawboundaries._max._x + cellsdelta._x,
                            ._y = _eng->_drawboundaries._max._y + cellsdelta._y};

    if (!COORD_MAXIMA_CHECK(_retracttest, _boundary_MAX))
        return ERANGE;
    else
    {
        _eng->_drawboundaries._max._x += cellsdelta._x;
        _eng->_drawboundaries._max._y += cellsdelta._y;

        _eng->_partboundaries._max._x += cellsdelta._x;
        _eng->_partboundaries._max._y += cellsdelta._y;
        return 0;
    }
    return EINVAL;
}

int engine_extend_drawing_area(struct ascii_2d_engine *_eng, coord_t extracells)
{
    if (!_eng)
        return EINVAL;

    coord_t _boundary_MAX = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y};
    coord_t _extenstiontest = {._x = _eng->_drawboundaries._max._x + extracells._x, ._y = _eng->_drawboundaries._max._y + extracells._y};

    if (!COORD_MAXIMA_CHECK(_extenstiontest, _boundary_MAX))
        return ERANGE;
    else
    {
        _eng->_drawboundaries._max._x += extracells._x;
        _eng->_drawboundaries._max._y += extracells._y;

        _eng->_partboundaries._max._x += extracells._x;
        _eng->_partboundaries._max._y += extracells._y;
        return 0;
    }
    return EINVAL;
}

int engine_draw_part_at(aoc_2d_engine_h _eng, coord_t *_pos, char *_sym)
{
    if (_eng->_enabledraw)
    {
        printf(MCUR_FMT "%s", _pos->_y, _pos->_x, _sym);
        engine_cursor_exit_drawing_area(_eng);
    }
    return 0;
}

int engine_draw(struct ascii_2d_engine *_eng)
{
    printf(ERASE_DISPLAY);
    printf(HOME);
    if (_eng->_enabledraw)
    {
        engine_draw_box(_eng);
        engine_fill_drawing_area(_eng);
        coord_t _pos = {0};
        aoc_engine_prompt_stats(_eng);
        engine_draw_objects(_eng, &_pos);
        engine_put_cursor_in_footer_area(_eng);
    }
    return 0;
}

int engine_draw_objects(aoc_2d_engine_h _eng, coord_t *_corner)
{
    if (_eng->_enabledraw)
    {
        printf("\n");
        LL_FOREACH(_obj_node, _eng->_objects)
        {
            struct object *_obj = CAST(struct object *, _obj_node);
            aoc_engine_draw_object(_eng, _obj, NULL);
        }
        printf("\n");
        usleep(1000 * _eng->_delay);
    }
    return 0;
}

int aoc_engine_append_obj(aoc_2d_engine_h engine, aoc_2d_object_h newobj)
{
    const coord_t _prevcoordlim = {._x = engine->_partboundaries._max._x, ._y = engine->_partboundaries._max._y};
    coord_t _newobjectXmaxYmax = {0};
    coord_t _extension = {0};

    LL_FOREACH(_partiter, newobj->_parts)
    {
        part_h _part = (part_h)_partiter;
        _newobjectXmaxYmax._x = HIGHEST(_newobjectXmaxYmax._x, _part->_pos._x + engine->_partboundaries._min._x);
        _newobjectXmaxYmax._y = HIGHEST(_newobjectXmaxYmax._y, _part->_pos._y + engine->_partboundaries._min._y);
    }

    if (_newobjectXmaxYmax._x > _prevcoordlim._x)
        _extension._x = _newobjectXmaxYmax._x - engine->_partboundaries._max._x;
    if (_newobjectXmaxYmax._y > _prevcoordlim._y)
        _extension._y = _newobjectXmaxYmax._y - engine->_partboundaries._max._y;

    if (dll_node_append(&engine->_objects, CAST(struct dll_node *, newobj)))
    {
        aoc_err("Failed to append %s", newobj->_name);
        return EINVAL;
    }

    if (_extension._x || _extension._y)
    {
        if (engine_extend_drawing_area(engine, _extension))
        {
            /* user has responsibility to free the unusable object */
            dll_node_disconnect(&engine->_objects, CAST(struct dll_node *, newobj));
            return ERANGE;
        }
        aoc_info("parts area extended up to x=%ld(+%ld) y=%ld(+%ld) to insert object %s", engine->_partboundaries._max._x, _extension._x, engine->_partboundaries._max._y, _extension._y, newobj->_name);
    }
    return 0;
}

/* will implement 2 methods, one by map and this one by objects linked list */
aoc_2d_object_h aoc_engine_get_obj_by_position(aoc_2d_engine_h _eng, coord_t *_pos)
{
    LL_FOREACH(_objNode, _eng->_objects)
    {
        object_h _object = CAST(object_h, _objNode);

        LL_FOREACH(_partnode, _object->_parts)
        {
            part_h part = (part_h)_partnode;
            coord_t *_partpos = &part->_pos;
            if (_partpos->_x == _pos->_x && _partpos->_y == _pos->_y)
            {
                return _object;
            }
        }
    }
    return NULL;
}

int aoc_engine_object_fit_detect(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = 0;

    coord_t _test = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    if (move_within_window(_eng, &_test, steps, dir))
        return EINVAL;
    aoc_2d_object_h _other = aoc_engine_get_obj_by_position(_eng, &_test);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_PROPERTY_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_engine_collision_at(aoc_2d_engine_h _eng, coord_t *_pos)
{
    if (!_eng)
        return EINVAL;
    aoc_2d_object_h _other = aoc_engine_get_obj_by_position(_eng, _pos);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_PROPERTY_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_engine_extend_one_direction(aoc_2d_engine_h _eng, size_t steps, AOC_2D_DIR _dir)
{
    if (!_eng || _dir >= AOC_DIR_MAX)
        return EINVAL;

    if (_dir > AOC_DIR_DOWN)
    {
        if ((_eng->_partboundaries._max._x + steps) > ABSOLUTE_MAX_X)
            return EINVAL;
        _eng->_partboundaries._max._x += steps;
        _eng->_drawboundaries._max._x += steps;
    }
    else
    {
        if ((_eng->_partboundaries._max._y + steps) > ABSOLUTE_MAX_Y)
            return EINVAL;
        _eng->_partboundaries._max._y += steps;
        _eng->_drawboundaries._max._y += steps;
    }
    LL_FOREACH(obj_node, _eng->_objects)
    {
        aoc_engine_step_object_and_redraw(_eng, CAST(aoc_2d_object_h, obj_node), steps, _dir, NULL);
    }
    return 0;
}

coordboundaries_t aoc_engine_get_parts_boundaries(aoc_2d_engine_h _eng)
{
    coordboundaries_t ret = {0};
    memcpy(&ret, &_eng->_partboundaries, sizeof(coordboundaries_t));
    return ret;
}

size_t aoc_engine_get_dist_between_objects(aoc_2d_engine_h _eng, aoc_2d_object_h _a, aoc_2d_object_h _b)
{
    coord_t _apos = aoc_engine_get_object_position(_eng, _a);
    coord_t _bpos = aoc_engine_get_object_position(_eng, _b);
    double _dX = HIGHEST(_apos._x, _bpos._x) - LOWEST(_apos._x, _bpos._x);
    double _dY = HIGHEST(_apos._y, _bpos._y) - LOWEST(_apos._y, _bpos._y);

    return (size_t)floor(sqrt((_dX * _dX) + (_dY * _dY)));
}

void engine_clear_screen()
{
    printf(ERASE_DISPLAY);
}

void engine_activate_drawing(aoc_2d_engine_h _eng)
{
    _eng->_enabledraw = true;
}

void engine_deactivate_drawing(aoc_2d_engine_h _eng)
{
    _eng->_enabledraw = false;
}