#include <aoc_2d_dynarr.h>
 
dyn2darr_h dyn2dmalloc(const coord_t bound, const size_t _csize)
{
    dyn2darr_h n2darr;
    TRY_TYPE_MALLOC(n2darr, dyn2darr_t); 
    n2darr->_bound = bound;
    n2darr->_csize = _csize;

    RANGE_FOR(itx, 0LU, bound._x + 1)
    {
        n2darr->_map[itx] = (void **)malloc((bound._y + 1) * _csize);
        RANGE_FOR(ity, 0LU, bound._y + 1)
        {
            n2darr->_map[itx][ity] = NULL;
        }
    }
    return n2darr;
} 

void dyn2dfree(dyn2darr_h o2darr)
{
    RANGE_FOR(itx, 0LU, o2darr->_bound._x + 1)
    {
        free(o2darr->_map[itx]);
    }
    free(o2darr->_map);
    free(o2darr);
}