#ifndef AOC_2D_DYNAMIC_ARRAY_H
#define AOC_2D_DYNAMIC_ARRAY_H
#include <aoc_helpers.h>
#include <aoc_coordinates.h>
#include <string.h>
#include <stdlib.h>

#define DYN2DMALLOC(_mem, _t, _x, _y)                 \
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

#define DYN2DMALLOC1BLOCK(_mem, _t, _x, _y)          \
    {                                                \
        _mem = malloc((_x) * (_y) * sizeof(_t *));   \
        memset(_mem, 0, (_x) * (_y) * sizeof(_t *)); \
    }

/**
 *
 *  I could show off with three stars pointers but pointer
 *  sizes are equal whichever the type
 *
 * */

#define DYNCUBICMALLOC(_mem, _t, _len)                            \
    {                                                             \
        _mem = malloc((_len) * sizeof(_t *));                     \
        RANGE_FOR(_itx, 0LU, (_len))                              \
        {                                                         \
            _mem[_itx] = malloc((_len) * sizeof(_t *));           \
            RANGE_FOR(_ity, 0LU, (_len))                          \
            {                                                     \
                _mem[_itx][_ity] = malloc((_len) * sizeof(_t *)); \
                RANGE_FOR(_itz, 0LU, (_len))                      \
                {                                                 \
                    _mem[_itx][_ity][_itz] = NULL;                \
                }                                                 \
            }                                                     \
        }                                                         \
    }

#define DYN2DFREE_DEALLOC(_mem, _t, _x, _y, _dealloc) \
    {                                                 \
        RANGE_FOR(_itx, 0LU, (_x))                    \
        {                                             \
            RANGE_FOR(_ity, 0LU, (_y))                \
            {                                         \
                if (_mem[_itx][_ity] && _dealloc)     \
                    _dealloc(_mem[_itx][_ity]);       \
            }                                         \
            free(_mem[_itx]);                         \
        }                                             \
        free(_mem);                                   \
    }

#define DYN2DFREE(_mem, _t, _x)    \
    {                              \
        RANGE_FOR(_itx, 0LU, (_x)) \
        {                          \
            free(_mem[_itx]);      \
        }                          \
        free(_mem);                \
    }

typedef struct dyn2darr
{
    void ***_map;
    coord_t _bound;
    size_t _csize;
} dyn2darr_t;

typedef dyn2darr_t *dyn2darr_h;

dyn2darr_h dyn2dmalloc(const coord_t bound, const size_t _csize);
void dyn2dfree(dyn2darr_h o2darr);

#endif