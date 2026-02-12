#include "zones.h"


typedef struct zone
{
    dll_head_t _edges;

} aoc_2d_zone_t;

/**
 *
 */

aoc_2d_zone_h aoc_2d_zone_ctor(void)
{
    aoc_2d_zone_h new = NULL;
    new = calloc(1LU, sizeof(aoc_2d_zone_t));
    return new;
}

typenode_ctor(edge, new->_pos = *pos, coord_t *pos);

int aoc_2d_zone_append(aoc_2d_zone_h zone_h, const edgenode_h const edge)
{
    LL_FOREACH(nedge, zone_h->_edges)
    {
        edgenode_h pedge = (edgenode_h)nedge;
        if ((edge->_prev->_x == pedge->_pos._x) || (edge->_prev->_y == pedge->_pos._y))
            pedge->_next = nedge;
        else
            pedge->_prev = nedge;
    }

    dll_node_append(&zone_h->_edges, edgenode_ctor(&edge->_pos));
}

int aoc_2d_isinsidezone(aoc_2d_zone_h zone, coord_h pos)
{
    float angle;
    LL_FOREACH(nedge, zone->_edges)
    {
        coordnode_h pedge = (coordnode_h)nedge;
        float deltang = (pedge->_coord._y - pos->_y) / (pedge->_coord._x - pos->_x);
        angle += deltang;
    }
}

void aoc_2d_zone_free(aoc_2d_zone_h zone)
{
    dll_free_all(&zone->_edges, free);
}