#ifndef AOC_2D_DYNAMIC_ARRAY_H
#define AOC_2D_DYNAMIC_ARRAY_H
#include <aoc_helpers.h>
#include <aoc_coordinates.h>
#include <string.h>
#include <stdlib.h>

#define DYN2DMALLOCM(_mem, _t, _x, _y)                \
    {                                                 \
        _mem = malloc((_x) * sizeof(_t *));           \
        RANGE_FOR(_itx, 0LU, (_x))                    \
        {                                             \
            _mem[_itx] = malloc((_y) * sizeof(_t *)); \
            RANGE_FOR(_ity, 0LU, (_y))                \
            {                                         \
                _mem[_itx][_ity] = NULL;              \
            }                                         \
        }                                             \
    }

#define DYN2DFREE_DEALLOC(_mem, _t, _x, _y, _dealloc) \
    {                                                 \
        RANGE_FOR(_itx, 0LU, (_x))                    \
        {                                             \
            RANGE_FOR(_ity, 0LU, (_y))                \
            {                                         \
                if (_mem[_itx][_ity] && _dealloc)     \
                    _dealloc(_mem[_itx][_ity]);       \
                else                                  \
                    _mem[_itx][_ity] = NULL;          \
            }                                         \
            free(_mem[_itx]);                         \
        }                                             \
        free(_mem);                                   \
    }

/**
 *
 *  I could show off with three stars pointers but pointer
 *  sizes are equal whichever the type
 *
 * */

#define DYN2DFREE(_mem, _t, _x)          \
    {                                    \
        RANGE_FOR(_itx, 0LU, (_x))       \
        {                                \
            RANGE_FOR(_ity, 0LU, (_y))   \
            {                            \
                _mem[_itx][_ity] = NULL; \
            }                            \
            free(_mem[_itx]);            \
        }                                \
        free(_mem);                      \
    }

typedef enum alloctype
{
    ALLOC_TYPE_1_BLOCK = 0,
    ALLOC_TYPE_M_BLOCK,
    ALLOC_TYPE_MAX,

} alloctype_t;

typedef struct dyn2darr
{
    void *_map;
    coord_t _len;
    size_t _csize;
    void (*_dealloc)(void *arg);
    alloctype_t _alloctype;
} dyn2darr_t;
typedef dyn2darr_t *dyn2darr_h;

typedef struct dyn3darr
{
    void *_map;
    size_t _len;
    void (*_dealloc)(void *arg);
    alloctype_t _alloctype;
} dyn3darr_t;
typedef dyn3darr_t *dyn3darr_h;

dyn2darr_h dyn2d1malloc(const coord_t _len, void dealloc(void *arg));
dyn2darr_h dyn2dMmalloc(const coord_t _len, void dealloc(void *arg));

dyn3darr_h dyn3d1malloc(const size_t _len, void dealloc(void *arg));
dyn3darr_h dyn3dMmalloc(const size_t _len, void dealloc(void *arg));

void **dyn2dget(dyn3darr_h array, coord_t *cell);
void **dyn3dget(dyn3darr_h array, uint3D_h cell);

void dyn2dfree(dyn2darr_h array);
void dyn3dfree(dyn3darr_h array);

#endif