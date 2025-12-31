#ifndef AOC_COORD_H
#define AOC_COORD_H

#include <stddef.h>
#include "aoc_linked_list.h"
#include "aoc_helpers.h"

#define COORD_RANGE_CHECK(_coord, _range) (N_BETWEEN_AB(_coord._x, _range._min._x, _range._max._x) && N_BETWEEN_AB(_coord._y, _range._min._y, _range._max._y))
#define COORD_RANGE_CHECK_P(_coord, _range) (N_BETWEEN_AB(_coord->_x, _range._min._x, _range._max._x) && N_BETWEEN_AB(_coord->_y, _range._min._y, _range._max._y))

typedef struct coord
{
    size_t _x;
    size_t _y;
} coord_t;
typedef coord_t *coord_h;

typedef struct coordpair
{
    coord_t _min;
    coord_t _max;
} coordpair_t;
typedef coordpair_t *coordpair_h;

typedef struct coordpairnode
{
    dll_node_t _node;
    coordpair_t _pair;
} coordpairnode_t;
typedef coordpairnode_t *coordpairnode_h;
dll_node_h coordpairnode_ctor(coordpair_h pair);
#define CORDPAIR(node) (((coordpairnode_h)node)->_pair)

typedef struct coordrefpairnode
{
    dll_node_t _node;
    coord_t *_first;
    coord_t *_second;
} coordrefpairnode_t;
typedef coordrefpairnode_t *coordrefpairnode_h;
dll_node_h coordrefpairnode_ctor(coordpair_h pair);

char *strpospair(const coordpair_h const pair);

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
int bscanuint2d(uint2D_t *array, uint2D_t *min, uint2D_t *max);
int bscanuint3d(uint3D_t *array, uint3D_t *min, uint3D_t *max);
/*
 * Coordinates comparators
 */

bool coordt_equal(void *arga, void *argb);

/** bool func(coordnode__t*, coordnode__t*) */
bool coordnodes_equal(void *_a, void *_b);
bool coord_higher_x(void *_a, void *_b);
bool coord_higher_y(void *_a, void *_b);
bool coord_lower_x(void *_a, void *_b);
bool coord_lower_y(void *_a, void *_b);
bool coordnodes_equal_x_lower_y(void *_a, void *_b);
bool coordnodes_equal_x_higher_y(void *_a, void *_b);
bool coord_dist_more_than_2(void *_a, void *_b);
bool coord_dist_less_than_2(void *_a, void *_b);
bool coord_on_adjacent_columns(void *_a, void *_b);

/**
 * dll_node_h func(coordpairnode__t*, coordpairnode__t*)
 * if true, insert _a before in dll;
 */
dll_node_h coordpair_equal_y_lower_x(dll_node_h _a, dll_node_h _b);
dll_node_h coordpair_equal_y_higher_x(dll_node_h _a, dll_node_h _b);
dll_node_h posnode_ydecr_xdecr(dll_node_h arga, dll_node_h argb);

/** bool func(coord_t*, size_t*) */
bool coord_same_column(void *arga, void *argb);

dll_node_h objref_xdecr_ydecr(dll_node_h arga, dll_node_h argb);
dll_node_h objref_ydecr_xdecr(dll_node_h arga, dll_node_h argb);

/**
 * dll_node_h func(dll_node_h, dll_node_h)
 * dll sorting will put arga before argb
 * if arga is returned.
 * if not it moves on to argb->_next
 * and tries again.
 * it it never returns arga, arga is appended
 * at the end of the ll;
 * */
dll_node_h highest_column(dll_node_h arga, dll_node_h argb);
dll_node_h coordnode_equal_x_higher_y(dll_node_h _a, dll_node_h _b);

dll_node_h coordnodes_equal_y_lower_x(dll_node_h arga, dll_node_h argb);
dll_node_h coord_y_aligned(dll_node_h arga, dll_node_h argb);

typedef struct coordnode
{
    struct dll_node _node;
    coord_t _coord;
} coordnode_t;

typedef coordnode_t *coordnode_h;

/*TODO make private*/
dll_node_h coordnode_ctor(coord_t *coord);
char *strpos(const coord_t *const pos);

#endif