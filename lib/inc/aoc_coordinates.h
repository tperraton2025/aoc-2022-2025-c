#ifndef AOC_COORD_H
#define AOC_COORD_H

#include <stddef.h>
#include "aoc_linked_list.h"
#include "aoc_helpers.h"

typedef struct coord
{
    size_t _x;
    size_t _y;
} coord_t;

typedef struct coordboundaries
{
    coord_t _min;
    coord_t _max;
} coordpair_t;

typedef union uint2D
{
    struct
    {
        size_t _x;
        size_t _y;
    } _xy;
    size_t _arr[2];
} uint2D_t;

typedef union uint3D
{
    struct
    {
        size_t _x;
        size_t _y;
        size_t _z;
    } _xyz;
    size_t _arr[3];
} uint3D_t;
typedef uint3D_t *uint3D_h;

uint3D_t uint3d_ctor(size_t x, size_t y, size_t z);
uint3D_t uint3d_sum(const uint3D_t *const a, const uint3D_t *const b);
bool uint3d_eq(const uint3D_t *const a, const uint3D_t *const b);

/**
 * 3D size_t indexes for regions
 */
const char *const struint3D(const uint3D_t *const a);
int adduint3D(uint3D_t *a, uint3D_t *b);

#define scanuint2d(arr, max) (scanuint, max, 2LU)
#define scanuint3d(arr, max) (scanuint, max, 3LU)

int scanuint(uint3D_t *array, size_t max, size_t dim);
int expuint3d(uint3D_t *array, uint3D_t *max);
int shruint3d(uint3D_t *array, uint3D_t *min);
int bscanuint3d(uint3D_t *array, uint3D_t *min, uint3D_t *max);
/*
 * Coordinates comparators
 */

/** bool func(coordtracker__t*, coordtracker__t*) */
bool coord_higher_x(void *_a, void *_b);
bool coord_higher_y(void *_a, void *_b);
bool coord_lower_x(void *_a, void *_b);
bool coord_lower_y(void *_a, void *_b);
bool coord_equal_x_lower_y(void *_a, void *_b);
bool coord_equal_x_higher_y(void *_a, void *_b);
bool coord_dist_more_than_2(void *_a, void *_b);
bool coord_dist_less_than_2(void *_a, void *_b);
bool coord_on_adjacent_columns(void *_a, void *_b);

/** bool func(coord_t*, size_t*) */
bool coord_same_column(void *arga, void *argb);

/** dll_node_h func(dll_node_h, dll_node_h) */
dll_node_h picklowestY(dll_node_h arga, dll_node_h argb);
dll_node_h highest_column(dll_node_h arga, dll_node_h argb);
dll_node_h objh_byXfirst(dll_node_h arga, dll_node_h argb);
dll_node_h objh_byYfirst(dll_node_h arga, dll_node_h argb);
dll_node_h pickhighestY(dll_node_h arga, dll_node_h argb);

typedef struct coord_tracker
{
    struct dll_node _node;
    coord_t _coord;
} coord_tracker_t;

typedef coord_tracker_t *coord_tracker_h;

/*TODO make private*/
dll_node_h coordtrackernode_ctor(coord_t *coord);
char *strpos(const coord_t *const pos);

bool coord_equal(void *_a, void *_b);
#endif