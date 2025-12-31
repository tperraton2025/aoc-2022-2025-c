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
    new = calloc(1LU, sizeof(aoc_2d_zone_t));
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
        (EDGE(newedge1))->_nextpoint = EDGE(newedge2)->_pos;
        (EDGE(newedge2))->_prevpoint = EDGE(newedge1)->_pos;

        dll_node_append(&zone_h->_newedges, newedge1);
        dll_node_append(&zone_h->_newedges, newedge2);
    }

    LL_FOREACH(pedge1, zone_h->_newedges)
    {
        LL_FOREACH(pedge2, zone_h->_newedges)
        {
            if ((EDGE(pedge1)->_pos._x == EDGE(pedge2)->_pos._x) &&
                (EDGE(pedge1)->_pos._y >= EDGE(pedge2)->_pos._y))
            {

                EDGE(pedge1)->_nextpoint = EDGE(pedge2)->_pos;
                EDGE(pedge2)->_prevpoint = EDGE(pedge1)->_pos;
            }
        }
    }

    return 0;
}

/**
 * @param points: dll of points delimitting the zone;
 */

int aoc_2d_zone_compile(aoc_2d_zone_h zone_h, dll_head_h points)
{
    LL_FOREACH_P(pnode, points) {
        
    }
}

int aoc_2d_isinsidezone(aoc_2d_zone_h zone, coord_h pos) {}

void aoc_2d_zone_free(aoc_2d_zone_h zone)
{
    dll_free_all(&zone->_newedges, free);
    dll_free_all(&zone->_pointlist, free);
}