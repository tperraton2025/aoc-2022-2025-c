#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <fcntl.h>

void engine_free(aoc_2d_eng_h _eng)
{
    dll_free_all(&_eng->_objects, aoc_2d_eng_free_obj);
    FREE(_eng);
}

DLL_NODE_CTOR(coord_tracker_t, coordtracker_ctor);

aoc_2d_eng_h aoc_2d_eng_create(coord_t *ncoordlimits, char _voidsym, size_t delay, dll_compare *comp)
{
    aoc_2d_eng_h _eng = NULL;
    TRY_RAII_MALLOC(_eng, sizeof(struct ascii_2d_engine));

    if (!_eng)
        return NULL;

    dll_head_init(&_eng->_objects);

    _eng->_delay = delay;
    _eng->_objsort = comp;

    if (COORD_RANGE_CHECK_P(ncoordlimits, _drawlimits))
    {
        _eng->_drawlimits._min._x = 4;
        _eng->_drawlimits._min._y = 4;

        _eng->_partoffset._x = _eng->_drawlimits._min._x + 1;
        _eng->_partoffset._y = _eng->_drawlimits._min._y + 1;

        _eng->_drawlimits._max._x = ncoordlimits->_x + _eng->_partoffset._x + 1;
        _eng->_drawlimits._max._y = ncoordlimits->_y + _eng->_partoffset._y + 1;

        _eng->_coordlimits._min._x = 0;
        _eng->_coordlimits._min._y = 0;

        _eng->_coordlimits._max._x = ncoordlimits->_x;
        _eng->_coordlimits._max._y = ncoordlimits->_y;

        _eng->_cursor._x = _eng->_coordlimits._min._x;
        _eng->_cursor._y = _eng->_coordlimits._min._y;

        _eng->_voidsym = _voidsym;

        _eng->_statLine = 2;
        _eng->_statCol = 0;
        _eng->_PrivStatLine = _eng->_statLine;

        _eng->_enablecollisions = true;

        _eng->_newlinecnt = _eng->_coordlimits._min._y;

        _eng->_enabledraw = _eng->_drawlimits._max._x < DRAWABLE_MAX_X && _eng->_drawlimits._max._y < DRAWABLE_MAX_Y;
        if (!_eng->_enabledraw)
            aoc_warn("%s", "drawable zone will not fit in a terminal window \n\
                drawing is disabled until forced");
        return _eng;
    }
    else
        goto cleanup;

cleanup:
    FREE(_eng);
    return NULL;
}

void aoc_2d_eng_parse_cli(aoc_2d_eng_h eng, int argc, char **argv)
{
    size_t canvassize = 0;
    size_t delay = 0;
    if (argc >= 2)
        for (int _iarg = 0; _iarg < argc; _iarg++)
        {
            if (0 == strcmp(argv[_iarg], "--no-draw"))
                aoc_2d_eng_disable_draw(eng);
            if ((0 == strcmp(argv[_iarg], "--draw-delay")) && argc > (_iarg + 1))
            {
                sscanf(argv[_iarg + 1], "%3lu", &delay);
                eng_set_refresh_delay(eng, delay);
            }
            if ((0 == strcmp(argv[_iarg], "--canvas-size")) && argc > (_iarg + 1))
            {
                sscanf(argv[_iarg + 1], "%3lu", &canvassize);
                if (128 < canvassize)
                {
                    aoc_info("canvas size is large (%ld), deactivating drawings for readability", canvassize);
                    aoc_2d_eng_disable_draw(eng);
                }
            }
        }

    eng_set_refresh_delay(eng, delay);
    aoc_2d_eng_extend_one_direction(eng, canvassize >> 1, AOC_2D_DIR_RIGHT);
    aoc_2d_eng_extend_one_direction(eng, canvassize >> 1, AOC_2D_DIR_DOWN);
}

void aoc_2d_eng_get_canvas_center(aoc_2d_eng_h _eng, coord_t *_pos)
{
    _pos->_x = (_eng->_coordlimits._max._x - _eng->_coordlimits._min._x) >> 1;
    _pos->_y = (_eng->_coordlimits._max._y - _eng->_coordlimits._min._y) >> 1;
}

int engine_extend_drawing_area(struct ascii_2d_engine *_eng, coord_t newlimits)
{
    if (!_eng)
        return EINVAL;

    if (!COORD_RANGE_CHECK(newlimits, _drawlimits))
        return ERANGE;
    else
    {
        if (_eng->_enabledraw)
            aoc_2d_eng_erase_stats(_eng);

        _eng->_drawlimits._max._x = newlimits._x + _eng->_partoffset._x + 1;
        _eng->_drawlimits._max._y = newlimits._y + _eng->_partoffset._y + 1;

        _eng->_coordlimits._max._x = newlimits._x;
        _eng->_coordlimits._max._y = newlimits._y;
        return 0;
    }
    return EINVAL;
}

int aoc_2d_eng_draw_part_at(aoc_2d_eng_h eng, coord_t *_pos, char *_sym, const char *fmt)
{
    if (eng->_enabledraw)
    {
        coord_t _pcord = {._x = _pos->_x + eng->_partoffset._x, ._y = _pos->_y + eng->_partoffset._y};
        aoc_2d_eng_draw_symbol_at(eng, &_pcord, _sym, fmt);
        aoc_2d_eng_exit_drawing_area(eng);
    }
    return 0;
}

int aoc_2d_eng_draw_symbol_at(aoc_2d_eng_h eng, coord_t *_pos, const char *_sym, const char *fmt)
{
    if (eng->_enabledraw)
    {
        printf(MCUR_FMT "%s%s\n" RESET, _pos->_y, _pos->_x, fmt, _sym);
        aoc_2d_eng_exit_drawing_area(eng);
        usleep(1000 * eng->_delay);
    }
    return 0;
}

int aoc_2d_eng_draw(struct ascii_2d_engine *_eng)
{
    if (_eng->_enabledraw)
    {
        size_t delay = _eng->_delay;
        _eng->_delay = 0;
        printf(ERASE_DISPLAY);
        printf(HOME);
        assert(COORD_RANGE_CHECK(_eng->_drawlimits._max, _drawlimits));
        assert(COORD_RANGE_CHECK(_eng->_drawlimits._min, _drawlimits));

        assert(COORD_RANGE_CHECK(_eng->_coordlimits._max, _coordboundaries));
        assert(COORD_RANGE_CHECK(_eng->_coordlimits._min, _coordboundaries));

        aoc_2d_eng_draw_box(_eng);
        engine_fill_drawing_area(_eng);
        coord_t _pos = {0};
        aoc_2d_eng_prompt_stats(_eng);
        aoc_2d_eng_prompt_obj_list(_eng);
        aoc_2d_eng_draw_objects(_eng);
        engine_put_cursor_in_footer_area(_eng);
        _eng->_delay = delay;
    }
    return 0;
}

int aoc_2d_eng_draw_objects(aoc_2d_eng_h _eng)
{
    if (_eng->_enabledraw)
    {
        printf("\n");
        LL_FOREACH(_obj_node, _eng->_objects)
        {
            if (_obj_node->_obsolete)
                continue;
            struct object *_obj = CAST(struct object *, _obj_node);
            aoc_2d_eng_draw_obj(_eng, _obj, NULL);
        }
        printf("\n");
    }
    return 0;
}

aoc_2d_eng_h aoc_2d_eng_obj_delete(aoc_2d_eng_h eng, struct object *obj)
{
    dll_node_disconnect(&eng->_objects, &obj->_node);
    aoc_2d_eng_free_obj(obj);
}

int aoc_2d_eng_append_obj(aoc_2d_eng_h engine, aoc_2d_obj_h newobj)
{
    coord_t _extension = {0};
    if (!newobj)
        return EINVAL;

    LL_FOREACH(_partiter, newobj->_parts)
    {
        part_h _part = (part_h)_partiter;
        _extension._x = HIGHEST(engine->_coordlimits._max._x, _part->_pos._x);
        _extension._y = HIGHEST(engine->_coordlimits._max._y, _part->_pos._y);
    }

    if (!COORD_RANGE_CHECK(_extension, _drawlimits))
        return ERANGE;

    int ret = dll_node_sorted_insert(&engine->_objects, CAST(dll_node_h, newobj), engine->_objsort);
    if (ret)
    {
        aoc_err("Failed to append %s at %s: %s", newobj->_name, strpos(&newobj->_pos), strerror(ret));
        return EINVAL;
    }

    if (_extension._x > engine->_coordlimits._max._x || _extension._y > engine->_coordlimits._max._y)
    {
        ret = engine_extend_drawing_area(engine, _extension);
        if (ret)
        {
            /* user has responsibility to free the unusable object */
            dll_node_disconnect(&engine->_objects, CAST(dll_node_h, newobj));
            aoc_err("Failed to append %s at %s: %s", newobj->_name, strpos(&newobj->_pos), strerror(ret));
            return ERANGE;
        }
    }
    aoc_2d_eng_draw_obj(engine, newobj, "");
    return 0;
}

/* will implement 2 methods, one by map and this one by objects linked list */
aoc_2d_obj_h aoc_2d_eng_get_obj_by_position(aoc_2d_eng_h _eng, coord_t *_pos)
{
    LL_FOREACH(_obj_node, _eng->_objects)
    {
        if (_obj_node->_obsolete)
            continue;
        aoc_2d_obj_h _object = CAST(aoc_2d_obj_h, _obj_node);
        if (_object->_pos._x == _pos->_x && _object->_pos._y == _pos->_y)
        {
            return _object;
        }

        // LL_FOREACH(_partnode, _object->_parts)
        //{
        //     part_h part = (part_h)_partnode;
        //     coord_t *_partpos = &part->_pos;
        //     if (_partpos->_x == _pos->_x && _partpos->_y == _pos->_y)
        //     {
        //         return _object;
        //     }
        // }
    }
    return NULL;
}

/* will implement 2 methods, one by map and this one by objects linked list */
dll_head_t *aoc_2d_eng_prompt_obj_list_with_a_part_at_position(aoc_2d_eng_h _eng, coord_t *_pos)
{
    dll_head_t *_list;
    dll_head_init(_list);
    LL_FOREACH(_obj_node, _eng->_objects)
    {
        if (_obj_node->_obsolete)
            continue;
        aoc_2d_obj_h _object = CAST(aoc_2d_obj_h, _obj_node);

        LL_FOREACH(_partnode, _object->_parts)
        {
            part_h part = (part_h)_partnode;
            coord_t *_partpos = &part->_pos;
            if (_partpos->_x == _pos->_x && _partpos->_y == _pos->_y)
            {
                aoc_2d_obj_ref_h _ref = aoc_2d_obj_ref(_object);
                dll_node_append(_list, &_ref->_node);
            }
        }
    }
    return _list;
}

int aoc_2d_obj_ctor_fit_detect(aoc_2d_eng_h _eng, aoc_2d_obj_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = 0;

    coord_t _test = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    if (move_within_window(_eng, &_test, steps, dir))
        return EINVAL;
    aoc_2d_obj_h _other = aoc_2d_eng_get_obj_by_position(_eng, &_test);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_FLAG_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_2d_eng_collision_at(aoc_2d_eng_h _eng, aoc_2d_obj_h excl, coord_t *_pos)
{
    if (!_eng)
        return EINVAL;
    aoc_2d_obj_h _other = aoc_2d_eng_get_obj_by_position(_eng, _pos);
    if (!_other || _other == excl)
        return 0;
    if (_other->_props & OBJ_FLAG_NO_COLLISION)
        return 0;
    return EEXIST;
}

int aoc_2d_eng_adapt_objects_to_extension(aoc_2d_eng_h _eng, size_t steps, AOC_2D_DIR _dir)
{
    int _ret = 0;
    bool _prevdrawsetting = _eng->_enabledraw;
    _eng->_enablecollisions = false;
    _eng->_enabledraw = false;
    LL_FOREACH(_obj_node, _eng->_objects)
    {
        if (_obj_node->_obsolete)
            continue;
        aoc_2d_obj_h _objh = CAST(aoc_2d_obj_h, _obj_node);
        for (size_t _it = 0; _it < steps; _it++)
        {
            _ret = aoc_2d_eng_step_obj(_eng, _objh, 1, _dir, NULL);
            if (_ret)
            {
                engine_put_cursor_in_footer_area(_eng);
                aoc_err("aoc_2d_eng_step_obj:%s %s", _objh->_name, strerror(_ret));
            }
        }
    }
    _eng->_enabledraw = _prevdrawsetting;
    _eng->_enablecollisions = true;
}

int aoc_2d_eng_extend_one_direction(aoc_2d_eng_h _eng, size_t steps, AOC_2D_DIR _dir)
{
    size_t delta = steps;
    if (!_eng || _dir >= AOC_2D_DIR_MAX)
        return EINVAL;
    int _ret = 0;
    if (_dir > AOC_2D_DIR_DOWN)
    {
        if ((_eng->_coordlimits._max._x + steps) > ABSOLUTE_MAX_X)
            return EINVAL;

        _eng->_coordlimits._max._x += steps;
        _eng->_drawlimits._max._x += steps;

        if (_dir == AOC_2D_DIR_LEFT)
            aoc_2d_eng_adapt_objects_to_extension(_eng, steps, AOC_2D_DIR_RIGHT);
    }
    else
    {
        if ((_eng->_coordlimits._max._y + steps) > ABSOLUTE_MAX_Y)
            return EINVAL;

        _eng->_coordlimits._max._y += steps;
        _eng->_drawlimits._max._y += steps;

        if (_dir == AOC_2D_DIR_UP)
            aoc_2d_eng_adapt_objects_to_extension(_eng, delta, AOC_2D_DIR_DOWN);
    }

    // if (_eng->_drawlimits._max._x > DRAWABLE_MAX_X)
    //     _eng->_enabledraw = false;
    // if (_eng->_drawlimits._max._y > DRAWABLE_MAX_Y)
    //     _eng->_enabledraw = false;
    return 0;
}

coordboundaries_t aoc_2d_eng_get_parts_boundaries(aoc_2d_eng_h _eng)
{
    coordboundaries_t ret = {0};
    memcpy(&ret, &_eng->_coordlimits, sizeof(coordboundaries_t));
    return ret;
}

size_t aoc_2d_eng_get_dist_between_objects(aoc_2d_eng_h _eng, aoc_2d_obj_h _a, aoc_2d_obj_h _b)
{
    coord_t _apos = aoc_2d_eng_get_obj_position(_eng, _a);
    coord_t _bpos = aoc_2d_eng_get_obj_position(_eng, _b);
    double _dX = HIGHEST(_apos._x, _bpos._x) - LOWEST(_apos._x, _bpos._x);
    double _dY = HIGHEST(_apos._y, _bpos._y) - LOWEST(_apos._y, _bpos._y);

    return (size_t)floor(sqrt((_dX * _dX) + (_dY * _dY)));
}

void engine_clear_screen()
{
    printf(ERASE_DISPLAY);
}

void aoc_2d_eng_enable_draw(aoc_2d_eng_h _eng)
{
    _eng->_enabledraw = true;
}

void aoc_2d_eng_disable_draw(aoc_2d_eng_h _eng)
{
    _eng->_enabledraw = false;
}

void engine_add_line(aoc_2d_eng_h _eng)
{
    assert(_eng->_newlinecnt < ABSOLUTE_MAX_Y);
    _eng->_newlinecnt++;
}

size_t engine_last_line(aoc_2d_eng_h _eng)
{
    return _eng->_newlinecnt % ABSOLUTE_MAX_Y;
}

int aoc_2d_eng_foreach_obj_arg(aoc_2d_eng_h _eng, void *arg, void func(coord_t *pos, void *arg))
{
    LL_FOREACH(_it, _eng->_objects)
    {
        if (_it->_obsolete)
            continue;
        aoc_2d_obj_h _objh = (aoc_2d_obj_h)_it;
        func(&_objh->_pos, arg);
    }
}

dll_head_h engine_get_objects_positions(aoc_2d_eng_h _eng)
{

    dll_head_h _poslists = malloc(sizeof(dll_head_t));
    dll_head_init(_poslists);

    LL_FOREACH(_objn, _eng->_objects)
    {
        if (_objn->_obsolete)
            continue;
        aoc_2d_obj_h _objh = (aoc_2d_obj_h)_objn;
        coord_tracker_h _ntr = coordtracker_ctor();
        _ntr->_coord._x = _objh->_pos._x;
        _ntr->_coord._y = _objh->_pos._y;
        dll_node_append(_poslists, &_ntr->_node);
    }
    return _poslists;
}

int engine_remove_obj(aoc_2d_eng_h eng, aoc_2d_obj_h obj)
{
    if (!eng || !obj)
        return EINVAL;
    aoc_2d_eng_erase_obj(eng, obj);
    dll_node_disconnect(&eng->_objects, &obj->_node);
    aoc_2d_eng_free_obj(&obj->_node);
    return 0;
}
