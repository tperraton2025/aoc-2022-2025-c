#include "aoc_helpers.h"
#include "engine.h"
#include "objects.h"
#include <assert.h>

const char defaultName[] = "no name";

/* creates a part that will be attached to an object.
a part has :
-   coordinates
-   can be used for collision detection when moving the object
-   its own symbol to represent the part itself
-   a format (color/underline/bold...)
*/

part_h aoc_2d_eng_new_part(struct object *obj, coord_t *sympos, char sym, char *fmt)
{
    part_h ret = malloc(sizeof(struct part));
    if (!ret)
        return NULL;
    memset(ret, 0, sizeof(struct part));
    if (fmt)
    {
        ret->_fmt = malloc(strnlen(fmt, ABSOLUTE_MAX_PART_FMT_LEN) + 1);
        if (!ret->_fmt)
            goto error;
    }
    ret->_pos._x = sympos->_x;
    ret->_pos._y = sympos->_y;
    ret->_sym = sym;
    return ret;
error:
    FREE(ret);
    return NULL;
}

void aoc_2d_eng_free_part(void *arg)
{
    part_h _parth = (part_h)arg;
    if (_parth->_fmt)
        FREE(_parth->_fmt);
    FREE(_parth);
}

void aoc_2d_eng_free_obj(void *_data)
{
    aoc_2d_obj_h _obj = (aoc_2d_obj_h)_data;
    /**
     * a reference counter would be useful to prevent dangling... references!
     */
    FREE(_obj->_name);
    FREE(_obj->_fmt);
    dll_free_all(&_obj->_parts, aoc_2d_eng_free_part);
    FREE(_obj);
}

void aoc_2d_eng_set_obj_flag(aoc_2d_obj_h obj, size_t flag)
{
    obj->_props |= flag;
    assert(obj->_props & flag);
}

void aoc_2d_eng_clear_obj_flag(aoc_2d_obj_h obj, size_t flag)
{
    obj->_props &= ~flag;
    assert(!(obj->_props & flag));
}

int aoc_2d_eng_calculate_obj_position(aoc_2d_obj_h obj)
{
    if (!obj)
        return EINVAL;
    coord_t _averagecoord = {0};
    LL_FOREACH(_partnode, obj->_parts)
    {
        part_h _part = (part_h)_partnode;
        _averagecoord._x += _part->_pos._x;
        _averagecoord._y += _part->_pos._y;
    }
    obj->_pos._x = _averagecoord._x / obj->_parts._size;
    obj->_pos._y = _averagecoord._y / obj->_parts._size;
    return 0;
}

const coord_t *const aoc_2d_obj_get_pos(aoc_2d_obj_h _obj)
{
    assert(_obj && "NULL pointers");
    return (const coord_t *const)&_obj->_pos;
}

aoc_2d_obj_ref_t *aoc_2d_obj_ref_ctor(aoc_2d_obj_h _obj)
{
    aoc_2d_obj_ref_t *_ntracker = NULL;
    TRY_TYPE_MALLOC(_ntracker, aoc_2d_obj_ref_t);

    if (!_ntracker)
        return NULL;
    _ntracker->_node._prev = NULL;
    _ntracker->_node._next = NULL;
    _ntracker->_blocked = false;
    _ntracker->data = _obj;
    _obj->_refcnt++;
    return _ntracker;
}

void aoc_2d_obj_ref_free(void *arg)
{
    aoc_2d_obj_ref_t *_ntracker = CAST(aoc_2d_obj_ref_t *, arg);
    _ntracker->data->_refcnt--;
    FREE(_ntracker);
}

int aoc_2d_obj_increment_collision_counter(aoc_2d_obj_h _obj)
{
    if (!_obj)
        return EINVAL;
    _obj->_collisionscounter++;
    return 0;
}

size_t aoc_2d_obj_get_collisioncounter(aoc_2d_obj_h _obj)
{
    return _obj->_collisionscounter;
}

const char *const aoc_2d_obj_get_name(aoc_2d_obj_h _obj)
{
    return _obj->_name;
}

static char _strobj[64] = {0};

char *strobj(aoc_2d_obj_h obj)
{
    sprintf(_strobj, "%.4s %.lu:%lu", obj->_name, obj->_pos._x, obj->_pos._y);
    return _strobj;
}

dll_node_h objh_byXfirst(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = &((aoc_2d_obj_h)arga)->_pos;
    coord_t *posb = &((aoc_2d_obj_h)argb)->_pos;
    if (posa->_x == posb->_x)
        return posa->_y > posb->_y ? arga : argb;
    return posa->_x > posb->_x ? arga : argb;
}

dll_node_h objh_byYfirst(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = &((aoc_2d_obj_h)arga)->_pos;
    coord_t *posb = &((aoc_2d_obj_h)argb)->_pos;
    if (posa->_y == posb->_y)
        return posa->_x > posb->_x ? arga : argb;
    return posa->_y > posb->_y ? arga : argb;
}

dll_node_h posh_byXfirst(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = ((coord_t *)arga);
    coord_t *posb = ((coord_t *)argb);
    if (posa->_x == posb->_x)
        return posa->_y > posb->_y ? arga : argb;
    return posa->_x > posb->_x ? arga : argb;
}

dll_node_h posh_byYfirst(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = ((coord_t *)arga);
    coord_t *posb = ((coord_t *)argb);
    if (posa->_y == posb->_y)
        return posa->_x > posb->_x ? arga : argb;
    return posa->_y > posb->_y ? arga : argb;
}

dll_node_h pickhighestY(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = &((aoc_2d_obj_h)arga)->_pos;
    coord_t *posb = &((aoc_2d_obj_h)argb)->_pos;
    return posa->_y > posb->_y ? arga : argb;
}

dll_node_h picklowestY(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = &((aoc_2d_obj_h)arga)->_pos;
    coord_t *posb = &((aoc_2d_obj_h)argb)->_pos;
    return posa->_y < posb->_y ? arga : argb;
}

const char *const aoc_2d_obj_name(const aoc_2d_obj_h const _obj)
{
    if (!_obj)
        return defaultName;
    return _obj->_name;
}