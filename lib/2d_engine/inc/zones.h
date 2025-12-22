#ifndef ASCII2D_ENG_ZONES_H
#define ASCII2D_ENG_ZONES_H
#include "aoc_linked_list.h"
#include "aoc_coordinates.h"
#include "aoc_ranges.h"

typedef struct zone *aoc_2d_zone_h; 

#define EDGECAST(node) ((edgenode_h)node)

aoc_2d_zone_h aoc_2d_zone_ctor(void);
int aoc_2d_zone_append(aoc_2d_zone_h zone_h, dll_head_h points);
int aoc_2d_zone_detect(aoc_2d_zone_h zone, coord_h pos);

#endif