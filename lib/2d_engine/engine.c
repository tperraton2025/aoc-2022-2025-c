#include "aoc_helpers.h"
#include "engine.h"
#include "objects.h"
#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <fcntl.h>

void engine_free(aoc_2d_eng_h eng)
{
    dll_free_all(&eng->_objects, aoc_2d_eng_free_obj);
    if (eng->_collisionmapenabled)
        freemap(eng);
    FREE(eng);
}

aoc_2d_eng_h aoc_2d_eng_create(coord_t *ncoordlimits, char _voidsym, size_t delay, dll_compare *comp, bool collisionmap)
{
    aoc_2d_eng_h eng = NULL;
    TRY_TYPE_MALLOC(eng, struct ascii_2d_engine);

    if (!eng)
        return NULL;

    if (eng->_drawingenabled)
        printf(ERASE_DISPLAY "\n");

    dll_head_init(&eng->_objects);

    eng->_delay = delay;
    eng->_objsort = comp;

    if (COORD_RANGE_CHECK_P(ncoordlimits, _drawlimits))
    {
        eng->_drawlimits._min._x = 4;
        eng->_drawlimits._min._y = 4;

        eng->_partoffset._x = eng->_drawlimits._min._x + 1;
        eng->_partoffset._y = eng->_drawlimits._min._y + 1;

        eng->_drawlimits._max._x = ncoordlimits->_x + eng->_partoffset._x + 1;
        eng->_drawlimits._max._y = ncoordlimits->_y + eng->_partoffset._y + 1;

        eng->_poslim._min._x = 0;
        eng->_poslim._min._y = 0;

        eng->_poslim._max._x = ncoordlimits->_x;
        eng->_poslim._max._y = ncoordlimits->_y;

        eng->_cursor._x = eng->_poslim._min._x;
        eng->_cursor._y = eng->_poslim._min._y;

        eng->_voidsym = _voidsym;

        eng->_statLine = 2;
        eng->_statCol = 0;
        eng->_PrivStatLine = eng->_statLine;

        eng->_newlinecnt = eng->_poslim._min._y;

        eng->_collisionsenabled = true;
        eng->_drawingenabled = eng->_drawlimits._max._x < DRAWABLE_MAX_X && eng->_drawlimits._max._y < DRAWABLE_MAX_Y;
        eng->_collisionmapenabled = collisionmap;

        if (collisionmap)
            allocmap(eng);

        if (!eng->_drawingenabled)
            aoc_warn("%s", "drawable zone will not fit in a terminal window \n\
                drawing is disabled until forced");
        return eng;
    }
    else
        goto cleanup;

cleanup:
    FREE(eng);
    return NULL;
}

void freemap(aoc_2d_eng_h eng)
{
    RANGE_FOR(_it, 0LU, eng->_objmap._cellcountx)
    {
        FREE(eng->_objmap._map[_it]);
    }
    FREE(eng->_objmap._map);
}

int allocmap(aoc_2d_eng_h eng)
{
    assert(eng);

    if (eng->_objmap._map)
        freemap(eng);

    objmap_h _objectsmap = &eng->_objmap;

    _objectsmap->_objcount = 0;
    _objectsmap->_cellcountx = 1 + eng->_poslim._max._x - eng->_poslim._min._x;
    _objectsmap->_cellcounty = 1 + eng->_poslim._max._y - eng->_poslim._min._y;

    _objectsmap->_map = malloc(_objectsmap->_cellcountx * sizeof(aoc_2d_obj_h *));
    RANGE_FOR(_it, 0LU, _objectsmap->_cellcountx)
    {
        _objectsmap->_map[_it] = malloc(_objectsmap->_cellcounty * sizeof(aoc_2d_obj_h));
        RANGE_FOR(_ity, 0LU, _objectsmap->_cellcounty)
        {
            _objectsmap->_map[_it][_ity] = NULL;
        }
    }
    LL_FOREACH(_node, eng->_objects)
    {
        aoc_2d_obj_h obj = (aoc_2d_obj_h)_node;

        assert(!_objectsmap->_map[obj->_pos._x][obj->_pos._y]);

        _objectsmap->_objcount++;
        _objectsmap->_map[obj->_pos._x][obj->_pos._y] = obj;
    }
    assert(_objectsmap->_objcount == eng->_objects._size);
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
                if (sscanf(argv[_iarg + 1], "%lu", &delay))
                    aoc_2d_eng_setrefreshdelay(eng, delay);
                else
                    aoc_err("%s", "--draw-delay not recognized, using default delay");
            }
            if ((0 == strcmp(argv[_iarg], "--canvas-size")) && argc > (_iarg + 1))
            {
                if (sscanf(argv[_iarg + 1], "%3lu", &canvassize))
                    if (128 < canvassize)
                    {
                        aoc_info("canvas size is large (%ld), deactivating drawings for readability", canvassize);
                        aoc_2d_eng_disable_draw(eng);
                    }
            }
        }

    aoc_2d_eng_setrefreshdelay(eng, delay);
    aoc_2d_eng_extend_one_direction(eng, canvassize >> 1, AOC_2D_DIR_RIGHT);
    aoc_2d_eng_extend_one_direction(eng, canvassize >> 1, AOC_2D_DIR_DOWN);
}

void aoc_2d_eng_get_canvas_center(aoc_2d_eng_h eng, coord_t *_pos)
{
    _pos->_x = (eng->_poslim._max._x - eng->_poslim._min._x) >> 1;
    _pos->_y = (eng->_poslim._max._y - eng->_poslim._min._y) >> 1;
}

int aoc_2d_eng_extend_drawing_area(struct ascii_2d_engine *eng, coord_t newlimits)
{
    if (!eng)
        return EINVAL;

    if (!COORD_RANGE_CHECK(newlimits, _drawlimits))
        return ERANGE;
    else
    {

        if (eng->_poslim._max._x < newlimits._x)
        {
            eng->_drawlimits._max._x = newlimits._x + eng->_partoffset._x + 1;
            eng->_poslim._max._x = newlimits._x;
        }

        if (eng->_poslim._max._y < newlimits._y)
        {
            eng->_drawlimits._max._y = newlimits._y + eng->_partoffset._y + 1;
            eng->_poslim._max._y = newlimits._y;
        }

        if (eng->_collisionmapenabled)
            allocmap(eng);
        return 0;
    }
    return EINVAL;
}

int aoc_2d_eng_draw_part_at(aoc_2d_eng_h eng, coord_t *_pos, char *_sym, const char *fmt)
{
    if (eng->_drawingenabled)
    {
        coord_t _pcord = {._x = _pos->_x + eng->_partoffset._x, ._y = _pos->_y + eng->_partoffset._y};
        printf(SAVECUR2);
        aoc_2d_eng_draw_symbol_at(eng, &_pcord, _sym, fmt);
        printf(RESTCUR2);
        // aoc_2d_eng_exit_drawing_area(eng);
        usleep(1000 * eng->_delay);
    }
    return 0;
}

int aoc_2d_eng_draw_symbol_at(aoc_2d_eng_h eng, coord_t *_pos, const char *_sym, const char *fmt)
{
    if (eng->_drawingenabled)
    {
        printf(MCUR_FMT "%s%s\n" RESET, _pos->_y, _pos->_x, fmt, _sym);
        aoc_2d_eng_exit_drawing_area(eng);
        usleep(1000 * eng->_delay);
    }
    return 0;
}

int aoc_2d_eng_draw(struct ascii_2d_engine *eng)
{
    if (eng->_drawingenabled)
    {
        size_t delay = eng->_delay;
        eng->_delay = 0;
        printf(ERASE_DISPLAY);
        printf(HOME);
        assert(COORD_RANGE_CHECK(eng->_drawlimits._max, _drawlimits));
        assert(COORD_RANGE_CHECK(eng->_drawlimits._min, _drawlimits));

        assert(COORD_RANGE_CHECK(eng->_poslim._max, _coordpair));
        assert(COORD_RANGE_CHECK(eng->_poslim._min, _coordpair));

        aoc_2d_eng_draw_box(eng);
        engine_fill_drawing_area(eng);
        coord_t _pos = {0};
        aoc_2d_eng_prompt_stats(eng);
        aoc_2d_eng_prompt_obj_list(eng);
        aoc_2d_eng_draw_objects(eng);
        engine_put_cursor_in_footer_area(eng);
        eng->_delay = delay;
    }
    return 0;
}

int aoc_2d_eng_draw_objects(aoc_2d_eng_h eng)
{
    if (eng->_drawingenabled)
    {
        printf("\n");
        LL_FOREACH(_obj_node, eng->_objects)
        {
            if (_obj_node->_obsolete)
                continue;
            struct object *_obj = CAST(struct object *, _obj_node);
            aoc_2d_eng_draw_obj(eng, _obj, NULL);
        }
        printf("\n");
    }
    return 0;
}

void aoc_2d_obj_delete(aoc_2d_eng_h eng, struct object *obj)
{
    aoc_2d_eng_erase_obj(eng, obj);
    dll_node_h prev = obj->_node._prev;
    dll_node_h curr = &obj->_node;
    dll_node_h next = obj->_node._next;

    dll_node_disconnect(&eng->_objects, &obj->_node);

    if (prev)
        assert(prev->_next == next);
    if (next)
        assert(next->_prev == prev);

    if (eng->_collisionmapenabled)
    {
        objmap_h _objectsmap = &eng->_objmap;

        _objectsmap->_objcount--;
        assert(_objectsmap->_objcount == eng->_objects._size);
        assert(_objectsmap->_map[obj->_pos._x][obj->_pos._y] == obj);
        _objectsmap->_map[obj->_pos._x][obj->_pos._y] = NULL;
    }

    aoc_2d_eng_free_obj(obj);
}

int aoc_2d_eng_append_obj(aoc_2d_eng_h eng, aoc_2d_obj_h newobj)
{
    coord_t _extension = {0};
    if (!newobj)
        return EINVAL;

    LL_FOREACH(_partnode, newobj->_parts)
    {
        part_h _part = (part_h)_partnode;
        _extension._x = HIGHEST(eng->_poslim._max._x, _part->_pos._x);
        _extension._y = HIGHEST(eng->_poslim._max._y, _part->_pos._y);
    }

    if (!COORD_RANGE_CHECK(_extension, _drawlimits))
        return ERANGE;

    int ret = 0;
    if (eng->_objsort)
        ret = dll_node_sorted_insert(&eng->_objects, (dll_node_h)newobj, eng->_objsort);
    else
        dll_node_append(&eng->_objects, (dll_node_h)newobj);
    if (ret)
    {
        aoc_err("Failed to append %s at %s: %s", newobj->_name, strpos(&newobj->_pos), strerror(ret));
        return EINVAL;
    }

    if (_extension._x > eng->_poslim._max._x || _extension._y > eng->_poslim._max._y)
    {
        ret = aoc_2d_eng_extend_drawing_area(eng, _extension);
        if (ret)
        {
            /* user has responsibility to free the unusable object */
            dll_node_disconnect(&eng->_objects, (dll_node_h)newobj);
            aoc_err("Failed to append %s at %s: %s", newobj->_name, strpos(&newobj->_pos), strerror(ret));
            return ERANGE;
        }
    }
    else if (eng->_collisionmapenabled)
    {
        objmap_h _objectsmap = &eng->_objmap;

        assert(!_objectsmap->_map[newobj->_pos._x][newobj->_pos._y]);

        _objectsmap->_map[newobj->_pos._x][newobj->_pos._y] = newobj;
        _objectsmap->_objcount++;
        assert(_objectsmap->_objcount == eng->_objects._size);
    }
    /**this cannot be used here as often the object population
     * phase affects the map size, wich complicates the
     * integrity of the image
     *
     *-->  aoc_2d_eng_draw_obj(eng, newobj, "");
     *
     */
    return 0;
}

/**
 * Implement 2 methods, one by map and this one by objects linked list
 * in cases with large maps and few objects, a linked list is advantageous to save RAM;
 * in cases with a large number of objects, a map imposes itself, regarless of the space it
 * takes, as collision detections requires to iterate an incalculable amount of times.
 */
aoc_2d_obj_h aoc_2d_eng_get_obj_by_position(aoc_2d_eng_h eng, const coord_t *const _pos)
{
    if (eng->_collisionmapenabled)
    {
        if ((_pos->_x <= eng->_poslim._max._x) &&
            (_pos->_y <= eng->_poslim._max._y))
            return eng->_objmap._map[_pos->_x][_pos->_y];
    }

    LL_FOREACH(_obj_node, eng->_objects)
    {
        if (_obj_node->_obsolete)
            continue;
        aoc_2d_obj_h _object = CAST(aoc_2d_obj_h, _obj_node);
        if (_object->_pos._x == _pos->_x && _object->_pos._y == _pos->_y)
        {
            return _object;
        }
    }
    return NULL;
}

/* will implement 2 methods, one by map and this one by objects linked list */
dll_head_t *aoc_2d_eng_prompt_obj_list_with_a_part_at_position(aoc_2d_eng_h eng, coord_t *_pos)
{
    dll_head_t *_list;
    dll_head_init(_list);
    LL_FOREACH(_obj_node, eng->_objects)
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
                aoc_2d_obj_ref_h _ref = aoc_2d_obj_ref_ctor(_object);
                dll_node_append(_list, &_ref->_node);
            }
        }
    }
    return _list;
}

int aoc_2d_obj_ctor_fit_detect(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = 0;

    coord_t _test = {._x = _obj->_pos._x, ._y = _obj->_pos._y};
    if (move_within_window(eng, &_test, steps, dir))
        return EINVAL;
    aoc_2d_obj_h _other = aoc_2d_eng_get_obj_by_position(eng, &_test);
    if (!_other)
        return 0;
    if (_other->_props & OBJ_FLAG_NO_COLLISION)
        return 0;
    return 1;
}

int aoc_2d_eng_collision_at(aoc_2d_eng_h eng, aoc_2d_obj_h excl, coord_t *_pos)
{
    if (!eng)
        return EINVAL;
    aoc_2d_obj_h _other = aoc_2d_eng_get_obj_by_position(eng, _pos);
    if (!_other || _other == excl)
        return 0;
    if (_other->_props & OBJ_FLAG_NO_COLLISION)
        return 0;
    _other->_collisionscounter++;
    return EEXIST;
}

int aoc_2d_eng_adapt_objects_to_extension(aoc_2d_eng_h eng, size_t steps, AOC_2D_DIR _dir)
{
    int _ret = 0;
    bool _prevdrawsetting = eng->_drawingenabled;
    eng->_collisionsenabled = false;
    eng->_drawingenabled = false;
    LL_FOREACH(_obj_node, eng->_objects)
    {
        if (_obj_node->_obsolete)
            continue;
        aoc_2d_obj_h _objh = CAST(aoc_2d_obj_h, _obj_node);
        for (size_t _it = 0; _it < steps; _it++)
        {
            _ret = aoc_2d_eng_step_obj(eng, _objh, 1, _dir, NULL);
            if (_ret)
            {
                engine_put_cursor_in_footer_area(eng);
                aoc_err("aoc_2d_eng_step_obj:%s %s", _objh->_name, strerror(_ret));
            }
        }
    }
    eng->_drawingenabled = _prevdrawsetting;
    eng->_collisionsenabled = true;
}

int aoc_2d_eng_extend_one_direction(aoc_2d_eng_h eng, size_t steps, AOC_2D_DIR _dir)
{
    size_t delta = steps;
    if (!eng || _dir >= AOC_2D_DIR_MAX)
        return EINVAL;
    int _ret = 0;
    if (_dir > AOC_2D_DIR_DOWN)
    {
        if ((eng->_poslim._max._x + steps) > ABSOLUTE_MAX_X)
            return EINVAL;

        eng->_poslim._max._x += steps;
        eng->_drawlimits._max._x += steps;

        if (_dir == AOC_2D_DIR_LEFT)
            aoc_2d_eng_adapt_objects_to_extension(eng, steps, AOC_2D_DIR_RIGHT);
    }
    else
    {
        if ((eng->_poslim._max._y + steps) > ABSOLUTE_MAX_Y)
            return EINVAL;

        eng->_poslim._max._y += steps;
        eng->_drawlimits._max._y += steps;

        if (_dir == AOC_2D_DIR_UP)
            aoc_2d_eng_adapt_objects_to_extension(eng, delta, AOC_2D_DIR_DOWN);
    }
    if (eng->_collisionmapenabled)
        allocmap(eng);
    return 0;
}

coordpair_t aoc_2d_eng_get_parts_boundaries(aoc_2d_eng_h eng)
{
    coordpair_t ret = {0};
    memcpy(&ret, &eng->_poslim, sizeof(coordpair_t));
    return ret;
}

size_t aoc_2d_eng_get_dist_between_objects(aoc_2d_eng_h eng, aoc_2d_obj_h _a, aoc_2d_obj_h _b)
{
    const coord_t *const _apos = aoc_2d_obj_get_pos(_a);
    const coord_t *const _bpos = aoc_2d_obj_get_pos(_b);
    double _dX = HIGHEST(_apos->_x, _bpos->_x) - LOWEST(_apos->_x, _bpos->_x);
    double _dY = HIGHEST(_apos->_y, _bpos->_y) - LOWEST(_apos->_y, _bpos->_y);

    return (size_t)floor(sqrt((_dX * _dX) + (_dY * _dY)));
}

void engine_clear_screen()
{
    printf(ERASE_DISPLAY);
}

void aoc_2d_eng_enable_draw(aoc_2d_eng_h eng)
{
    eng->_drawingenabled = true;
}

void aoc_2d_eng_disable_draw(aoc_2d_eng_h eng)
{
    eng->_drawingenabled = false;
}

void aoc_2d_eng_suspend_draw(aoc_2d_eng_h eng)
{
    eng->_prevenabledraw = eng->_drawingenabled;
    eng->_drawingenabled = false;
}

void aoc_2d_eng_unsuspend_draw(aoc_2d_eng_h eng)
{
    eng->_drawingenabled = eng->_prevenabledraw;
}

void engine_add_line(aoc_2d_eng_h eng)
{
    assert(eng->_newlinecnt < ABSOLUTE_MAX_Y);
    eng->_newlinecnt++;
}

size_t engine_last_line(aoc_2d_eng_h eng)
{
    return eng->_newlinecnt % ABSOLUTE_MAX_Y;
}

void aoc_2d_eng_foreach_objpos_arg(aoc_2d_eng_h eng, void *arg, bool func(void *arga, void *argb))
{
    LL_FOREACH(_it, eng->_objects)
    {
        if (_it->_obsolete)
            continue;
        aoc_2d_obj_h _obj = (aoc_2d_obj_h)_it;
        coordnode_t _temp = {0};
        _temp._coord._x = _obj->_pos._x;
        _temp._coord._y = _obj->_pos._y;
        if (!func(arg, &_temp))
            break;
    }
}

dll_head_h aoc_2d_eng_list_objects(aoc_2d_eng_h eng, const char *const incl, const char *const excl)
{

    dll_head_h _objlist = malloc(sizeof(dll_head_t));
    dll_head_init(_objlist);

    LL_FOREACH(_objn, eng->_objects)
    {
        if (_objn->_obsolete)
            continue;
        aoc_2d_obj_h objh = (aoc_2d_obj_h)_objn;

        if (incl && excl)
        {
            if (!strcmp(objh->_name, incl) && strcmp(objh->_name, excl))
            {
                aoc_2d_obj_ref_h _ntr = aoc_2d_obj_ref_ctor(objh);
                dll_node_append(_objlist, &_ntr->_node);
            }
        }
        else
        {
            aoc_2d_obj_ref_h _ntr = aoc_2d_obj_ref_ctor(objh);
            dll_node_append(_objlist, &_ntr->_node);
        }
    }
    return _objlist;
}

dll_head_h aoc_2d_eng_list_obj_pos(aoc_2d_eng_h eng, dll_compare *comp)
{

    dll_head_h _poslists = malloc(sizeof(dll_head_t));
    dll_head_init(_poslists);

    LL_FOREACH(node, eng->_objects)
    {
        if (node->_obsolete)
            continue;
        dll_node_sorted_insert(_poslists, coordnode_ctor(&((aoc_2d_obj_h)node)->_pos), comp);
    }
    return _poslists;
}

/*
  TODO: will improve with dll_get_nodes_by_property in the future
 */
dll_head_h aoc_2d_eng_list_obj_pos_arg(aoc_2d_eng_h eng, void *arg, bool func(void *arga, void *argb))
{
    dll_head_h _poslists = malloc(sizeof(dll_head_t));
    dll_head_init(_poslists);

    LL_FOREACH(_objn, eng->_objects)
    {
        aoc_2d_obj_h _objh = (aoc_2d_obj_h)_objn;
        if (_objn->_obsolete)
            continue;
        if (!func(arg, _objn))
            continue;
        dll_node_append(_poslists, coordnode_ctor(&_objh->_pos));
    }
    return _poslists;
}

int engine_remove_obj(aoc_2d_eng_h eng, aoc_2d_obj_h obj)
{
    if (!eng || !obj)
        return EINVAL;
    aoc_2d_eng_erase_obj(eng, obj);
    if (eng->_collisionmapenabled)
    {
        eng->_objmap._objcount--;
        eng->_objmap._map[obj->_pos._x][obj->_pos._y] = NULL;
    }
    dll_node_disconnect(&eng->_objects, &obj->_node);
    aoc_2d_eng_free_obj(&obj->_node);
    return 0;
}

bool aoc_2d_eng_isdrawing(aoc_2d_eng_h eng)
{
    return eng->_drawingenabled;
}

part_h aoc_2d_eng_get_part_by_position(aoc_2d_eng_h eng, coord_t *pos)
{
    LL_FOREACH(_objnode, eng->_objects)
    {
        aoc_2d_obj_h _obj = (aoc_2d_obj_h)_objnode;
        LL_FOREACH(_partnode, _obj->_parts)
        {
            part_h _part = (part_h)_partnode;
            if (pos->_x == _part->_pos._x && pos->_x == _part->_pos._x)
                return _part;
        }
    }
    return NULL;
}

int aoc_2d_eng_move_all_parts(aoc_2d_eng_h eng, aoc_2d_obj_h obj, size_t steps, AOC_2D_DIR dir)
{
    LL_FOREACH(_partnode, obj->_parts)
    {
        part_h _part = (part_h)_partnode;
        coord_t _prevpos = {._x = _part->_pos._x, ._y = _part->_pos._y};

        move_within_coord(eng, &_part->_pos, steps, dir);

        /** huge bottleneck
         * part_h other = aoc_2d_eng_get_part_by_position(eng, &_prevpos);
         * if (other)
         *     aoc_2d_eng_draw_part(eng, other, NULL);
         */
    }
    return 0;
}

static int aoc_2d_eng_check_move_possible(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, size_t steps, AOC_2D_DIR dir)
{
    int ret = 0;
    LL_FOREACH(_partnode, _obj->_parts)
    {
        part_h _part = (part_h)_partnode;
        coord_t _testpos = {._x = _part->_pos._x, ._y = _part->_pos._y};
        ret = move_within_coord(eng, &_testpos, steps, dir);
        if (ret)
        {
            aoc_2d_eng_prompt_multistr(eng, 0, _obj->_name, ": out of box at ", strpos(&_testpos));
            break;
        }
        if (eng->_collisionsenabled && !(_obj->_props & OBJ_FLAG_NO_COLLISION))
        {
            ret = aoc_2d_eng_collision_at(eng, _obj, &_testpos);
            if (EEXIST == ret)
                _obj->_collisionscounter++;
        }
        if (ret)
        {
            break;
        }
    }
    return ret;
}

int aoc_2d_eng_step_obj(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, size_t steps, AOC_2D_DIR dir, char *fmt)
{
    int _ret = aoc_2d_eng_check_move_possible(eng, _obj, steps, dir);
    if (_ret)
        return _ret;

    if (eng->_drawingenabled)
        if (_obj->_props & OBJ_FLAG_TRACEING)
            aoc_2d_eng_draw_obj(eng, _obj, fmt);
        else
            aoc_2d_eng_erase_obj(eng, _obj);

    _ret = aoc_2d_eng_move_all_parts(eng, _obj, steps, dir);
    if (_ret)
    {
        engine_put_cursor_in_footer_area(eng);
        aoc_err("aoc_2d_eng_move_all_parts:%s", strerror(_ret));
    }
    coord_t prevpos = _obj->_pos;
    _ret = move_within_coord(eng, &_obj->_pos, steps, dir);
    if (_ret)
    {
        engine_put_cursor_in_footer_area(eng);
        aoc_err("move_within_coord:%s", strerror(_ret));
    }
    else if (eng->_collisionmapenabled && eng->_collisionsenabled)
    {
        aoc_2d_obj_h check = eng->_objmap._map[prevpos._x][prevpos._y];

        assert(check == _obj);
        assert(!strncmp(check->_name, _obj->_name, MAX_NAME_LEN_LUI));

        eng->_objmap._map[_obj->_pos._x][_obj->_pos._y] = eng->_objmap._map[prevpos._x][prevpos._y];
        eng->_objmap._map[prevpos._x][prevpos._y] = NULL;
    }
    if (eng->_drawingenabled)
        _ret = aoc_2d_eng_draw_obj(eng, _obj, fmt);
    if (_ret)
    {
        engine_put_cursor_in_footer_area(eng);
        aoc_err("aoc_2d_eng_draw_object:%s", strerror(_ret));
    }
    engine_put_cursor_in_footer_area(eng);
    return _ret;
}


int aoc_2d_eng_draw_part(struct ascii_2d_engine *eng, part_h part, char *specfmt)
{
    int ret = 0;
    if (eng->_drawingenabled)
    {
        printf(MCUR_FMT "%s", part->_pos._y + eng->_partoffset._y, part->_pos._x + eng->_partoffset._x, specfmt ? specfmt : "");
        printf("%c\n" RESET, part->_sym);
    }
    return ret;
}

int aoc_2d_eng_draw_obj(aoc_2d_eng_h eng,  aoc_2d_obj_h obj, char *specfmt)
{
    int ret = 0;

    if (eng->_drawingenabled)
    {
        dll_node_h _partdllnode;
        part_h _parth = NULL;
        LL_FOREACH_EXT(_partdllnode, obj->_parts)
        {
            _parth = CAST(part_h, _partdllnode);
            aoc_2d_eng_draw_part(eng, _parth, specfmt ? specfmt : obj->_fmt);
        }
        usleep(1000 * eng->_delay);
        aoc_2d_eng_exit_drawing_area(eng);
    }

    return ret;
}


int aoc_2d_eng_erase_obj(struct ascii_2d_engine *eng, struct object *obj)
{
    int _ret = 0;

    if (eng->_drawingenabled)
    {
        dll_node_h _part_node;
        part_h _sym;
        LL_FOREACH_EXT(_part_node, obj->_parts)
        {
            _sym = CAST(part_h, _part_node);
            printf(MCUR_FMT "%c", _sym->_pos._y + eng->_partoffset._y, _sym->_pos._x + eng->_partoffset._x, eng->_voidsym);
        }
        aoc_2d_eng_exit_drawing_area(eng);
    }
    return _ret;
}

aoc_2d_obj_h aoc_2d_eng_get_obj_by_name(aoc_2d_eng_h eng, const char *const name)
{
    LL_FOREACH(_node, eng->_objects)
    {
        if (_node->_obsolete)
            continue;
        if (!strcmp(name, CAST(aoc_2d_obj_h, _node)->_name))
            return CAST(aoc_2d_obj_h, _node);
    }
    return NULL;
}

int aoc_2d_eng_put_obj_and_redraw(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, coord_t _npos)
{
    if (!eng || !_obj)
        return EINVAL;

    int ret = is_position_in_box(eng, &_npos);
    if (ret)
        return ret;

    ret = aoc_2d_eng_collision_at(eng, _obj, &_npos);
    if (ret)
        return ret;

    aoc_2d_eng_erase_obj(eng, _obj);
    put_pos(eng, &_obj->_pos, &_npos);
    aoc_2d_eng_draw_obj(eng, _obj, NULL);
    engine_put_cursor_in_footer_area(eng);

    if (eng->_collisionmapenabled)
        eng->_objmap._map[_npos._x][_npos._y] = _obj;

    return 0;
}

void aoc_2d_eng_disable_collisions(aoc_2d_eng_h eng)
{
    eng->_collisionsenabled = false;
}

void aoc_2d_eng_enable_collisions(aoc_2d_eng_h eng)
{
    eng->_collisionsenabled = true;
}
int aoc_2d_eng_move_one_step_towards(aoc_2d_eng_h eng, aoc_2d_obj_h _a, coord_t _pos)
{
    int ret = 0;
    const coord_t *const current = aoc_2d_obj_get_pos(_a);
    if (current->_x < _pos._x)
        ret = aoc_2d_eng_step_obj(eng, _a, 1LU, AOC_2D_DIR_RIGHT, NULL);
    if (current->_x > _pos._x && !ret)
        ret = aoc_2d_eng_step_obj(eng, _a, 1LU, AOC_2D_DIR_LEFT, NULL);
    if (current->_y < _pos._y && !ret)
        ret = aoc_2d_eng_step_obj(eng, _a, 1LU, AOC_2D_DIR_DOWN, NULL);
    if (current->_y > _pos._y && !ret)
        ret = aoc_2d_eng_step_obj(eng, _a, 1LU, AOC_2D_DIR_UP, NULL);
    return ret;
}
size_t aoc_2d_eng_get_obj_count(aoc_2d_eng_h eng)
{
    return eng->_objects._size % LL_MAX_LEN_LUI;
}

char _strobjcountbuf[] = LL_MAX_LEN_STR;
char *strobjcnt(aoc_2d_eng_h eng)
{
    snprintf(_strobjcountbuf, ARRAY_DIM(_strobjcountbuf), "%lu", aoc_2d_eng_get_obj_count(eng));
    return _strobjcountbuf;
}

int aoc_2d_eng_clear_all_collision_counters(aoc_2d_eng_h eng)
{
    if (!eng)
        return EINVAL;
    LL_FOREACH(_node, eng->_objects)
    {
        aoc_2d_obj_h obj = (aoc_2d_obj_h)_node;
        obj->_collisionscounter = 0;
    }
    return 0;
}