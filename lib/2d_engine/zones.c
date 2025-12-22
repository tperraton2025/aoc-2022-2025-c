#include "zones.h"

typedef_dllnode(edge,
                coord_t _pos;
                coord_t _prevpoint;
                coord_t _nextpoint;);

typedef struct zone
{
    dll_head_t _newedges;
    dll_head_t _pointlist;

} aoc_2d_zone_t;

aoc_2d_zone_h aoc_2d_zone_ctor(void)
{
    aoc_2d_zone_h new = NULL;
    TRY_TYPE_MALLOC(new, aoc_2d_zone_t);
    return new;
}

typenode_ctor(edge, new->_pos = *pos, coord_t *pos);

/**
 * @param points: dll of points pair connected together horizontally;
 */
int aoc_2d_zone_append(aoc_2d_zone_h zone_h, dll_head_h points)
{
    LL_FOREACH_P(pnode, points)
    {
        dll_node_h newedge1 = edgenode_ctor(&CORDPAIR(pnode)._max);
        dll_node_h newedge2 = edgenode_ctor(&CORDPAIR(pnode)._min);
        (EDGECAST(newedge1))->_nextpoint = EDGECAST(newedge2)->_pos;
        (EDGECAST(newedge2))->_prevpoint = EDGECAST(newedge1)->_pos;

        dll_node_append(&zone_h->_newedges, newedge1);
        dll_node_append(&zone_h->_newedges, newedge2);
    }

    LL_FOREACH(pedge1, zone_h->_newedges)
    {
        LL_FOREACH(pedge2, zone_h->_newedges)
        {
            if ((EDGECAST(pedge1)->_pos._x == EDGECAST(pedge2)->_pos._x) &&
                (EDGECAST(pedge1)->_pos._y >= EDGECAST(pedge2)->_pos._y))
            {

                EDGECAST(pedge1)->_nextpoint = EDGECAST(pedge2)->_pos;
                EDGECAST(pedge2)->_prevpoint = EDGECAST(pedge1)->_pos;
            }
        }
    }

    return 0;
}

/**
 * @param points: dll of points pair connected together horizontally;
 */
int aoc_2d_zone_compile(aoc_2d_zone_h zone_h)
{
    dll_node_append(&zone_h->_pointlist, zone_h->_newedges._first);
    dll_node_disconnect(&zone_h->_pointlist, zone_h->_newedges._first);

    LL_FOREACH(pedge, zone_h->_newedges)
    {
        dll_node_insert_after(&zone_h->_pointlist,
                              pedge,
                              dll_node_find_by_property(&zone_h->_newedges, &EDGECAST(pedge)->_prevpoint, coordt_equal));
    }

    return 0;
}
int aoc_2d_zone_detect(aoc_2d_zone_h zone, coord_h pos) {}

void aoc_2d_zone_free(aoc_2d_zone_h zone)
{
}