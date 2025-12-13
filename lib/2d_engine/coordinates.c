#include "aoc_coordinates.h"
#include "aoc_ranges.h"

bool coord_dist_more_than_2(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    size_t disty = ABSDIFF(_acord->_y, _bcord->_y);
    return distx > 2 && disty > 2;
}

bool coord_dist_less_than_2(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    size_t disty = ABSDIFF(_acord->_y, _bcord->_y);
    return distx < 2 && disty < 2;
}

bool coord_on_adjacent_columns(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    return distx < 2;
}

bool coord_higher_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return _bcord->_y > _acord->_y;
}

bool coord_lower_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return _bcord->_y < _acord->_y;
}

bool coord_x_equal(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return _acord->_x == _bcord->_x;
}

/** return true if _b point has higher Y on same X */
bool coord_equal_x_higher_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return ((_bcord->_y > _acord->_y) && (_acord->_x == _bcord->_x));
}

/** return true if _b point has higher Y on same X */
bool coord_equal_x_lower_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    // if ((_bcord->_y < _acord->_y) && (_bcord->_x == _acord->_x))
    //{
    //     printf(BRED "removing %s", strpos(_acord));
    //     printf("for %s\n" RESET, strpos(_bcord));
    // }
    return ((_bcord->_y < _acord->_y) && (_bcord->_x == _acord->_x));
}

bool coord_same_column(void *arga, void *argb)
{
    coord_t *_pos = (coord_t *)argb;
    return _pos->_x == (size_t)arga;
}

dll_node_h highest_column(dll_node_h arga, dll_node_h argb)
{
    coord_t *_posa = (coord_t *)arga;
    coord_t *_posb = (coord_t *)argb;
    return (dll_node_h)(_posa->_x > _posb->_x ? _posa : _posb);
}

DLL_NODE_CTOR(coord_tracker_t, coordtrackernode_ctor)

coord_tracker_h coordtracker_ctor(coord_t *coord)
{
    coord_tracker_h new = coordtrackernode_ctor();
    new->_coord._x = coord->_x;
    new->_coord._y = coord->_y;
    return new;
}

static char _strpos[] = "[00000x00000]";
/*! warning: not thread safe */
char *strpos(const coord_t *const pos)
{
    snprintf(_strpos, ARRAY_DIM(_strpos), "[%*.1lu x%*.1lu]", 4, pos->_x, 4, pos->_y);
    return _strpos;
}
