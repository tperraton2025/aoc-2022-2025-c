#include <aoc_dynamic_array.h>

dyn2darr_h dyn2d1malloc(const coord_t _len, void dealloc(void *arg))
{
    dyn2darr_h array;
    TRY_TYPE_MALLOC(array, dyn2darr_t);
    array->_len = _len;
    array->_dealloc = dealloc;
    array->_alloctype = ALLOC_TYPE_1_BLOCK;

    array->_map = malloc((_len._y + 1) * (_len._x + 1) * sizeof(void *));
    memset(array->_map, 0, (_len._y + 1) * (_len._x + 1) * sizeof(void *));

    return array;
}

dyn2darr_h dyn2dMmalloc(const coord_t _len, void dealloc(void *arg))
{
    dyn2darr_h array;
    TRY_TYPE_MALLOC(array, dyn2darr_t);
    array->_len = _len;
    array->_dealloc = dealloc;
    array->_alloctype = ALLOC_TYPE_M_BLOCK;

    array->_map = malloc((_len._y) * sizeof(void *));
    RANGE_FOR(_ity, 0LU, (_len._y))
    {
        ((void ***)array->_map)[_ity] = malloc((_len._x) * sizeof(void *));
        RANGE_FOR(_itx, 0LU, (_len._x))
        {
            ((void ***)array->_map)[_ity][_itx] = NULL;
        }
    }
    return array;
}

void dyn2dfree(dyn2darr_h array)
{
    void *cellp = NULL;
    uint2D_t scan = {0};
    do
    {
        if (array->_alloctype == ALLOC_TYPE_1_BLOCK)
            cellp = ((void ***)array->_map)[scan._xy._x][scan._xy._y];
        else if (array->_alloctype == ALLOC_TYPE_M_BLOCK)
            cellp = ((void **)array->_map)[scan._xy._x + (scan._xy._y * array->_len._x)];
        else
            assert(false);

        if (array->_dealloc && cellp)
            array->_dealloc(cellp);
        else
            cellp = NULL;
        if (array->_alloctype == ALLOC_TYPE_M_BLOCK)
        {
            cellp = ((void **)array->_map)[scan._xy._y - 1];
            if (scan._xy._y && !scan._xy._x && cellp)
                free(cellp);
        }
    } while (!scanuint_array(scan._arr, 0LU, array->_len._x, ARRAY_DIM(scan._arr)));

    FREE(array->_map);
    FREE(array);
}

dyn3darr_h dyn3d1malloc(const size_t _len, void dealloc(void *arg))
{
    dyn3darr_h array;
    TRY_TYPE_MALLOC(array, dyn3darr_t);
    array->_len = _len;
    array->_dealloc = dealloc;
    array->_alloctype = ALLOC_TYPE_1_BLOCK;

    size_t blocksize = (_len + 1) * (_len + 1) * (_len + 1) * sizeof(void *);
    array->_map = malloc(blocksize);
    assert(array->_map);
    memset(array->_map, 0, blocksize);

    uint3D_t scan = {0};
    do
    {
        void **test = dyn3dget(array, &scan);
        assert(*test == NULL);
    } while (!scanuint_array(scan._arr, 0LU, array->_len, ARRAY_DIM(scan._arr)));

    return array;
}

/** only for the sake of actual understanding. The number of allocation is a strong penalty */
dyn3darr_h dyn3dMmalloc(const size_t _len, void dealloc(void *arg))
{
    dyn3darr_h array;
    TRY_TYPE_MALLOC(array, dyn3darr_t);
    array->_len = _len;
    array->_dealloc = dealloc;
    array->_alloctype = ALLOC_TYPE_M_BLOCK;
    const size_t arraydim = (_len + 1) * sizeof(void *);

    /** allocation for all pointers on z axis */
    array->_map = malloc(arraydim);
    memset(array->_map, 0, arraydim);

    /** now it starts (T-T) */
    void *****map = (void *****)&array->_map;

    RANGE_FOR(itz, 0LU, (_len + 1))
    {
        /** allocation for all pointers on y axis */
        (*map)[itz] = malloc(arraydim);
        memset((*map)[itz], 0, _len);

        RANGE_FOR(ity, 0LU, (_len + 1))
        {
            /** allocation for all pointers on x axis */
            (*map)[itz][ity] = malloc(arraydim);
            memset((*map)[itz][ity], 0, _len);
        }
    }
    /** (x-x) */

    return array;
}

void dyn3dfree(dyn3darr_h array)
{
    void **cellp = NULL;
    uint3D_t scan = {0};
    do
    {
        if (array->_alloctype == ALLOC_TYPE_M_BLOCK)
            cellp = &((void ****)array->_map)[scan._xyz._z][scan._xyz._y][scan._xyz._x];
        else if (array->_alloctype == ALLOC_TYPE_1_BLOCK)
            cellp = &((void **)array->_map)[(scan._xyz._z) +
                                            (scan._xyz._y * array->_len) +
                                            (scan._xyz._x * array->_len * array->_len)];
        else
            assert(false);

        if (array->_dealloc && *cellp)
        {
            array->_dealloc(*cellp);
            *cellp = NULL;
        }
        else
            cellp = NULL;
        if (array->_alloctype == ALLOC_TYPE_M_BLOCK)
        {
            /** There is a memory leak to fix, as ALLOC_TYPE_M_BLOCK is way too slow it can wait */
            if (scan._xyz._y && !scan._xyz._x)
                free(((void ****)array->_map)[scan._xyz._z][scan._xyz._y - 1]);
            if (scan._xyz._z && !scan._xyz._y && !scan._xyz._x)
                free(((void ****)array->_map)[scan._xyz._z - 1]);
        }
    } while (!scanuint_array(scan._arr, 0LU, array->_len, ARRAY_DIM(scan._arr)));

    FREE(array->_map);
    FREE(array);
}

void **dyn3dget(dyn3darr_h array, uint3D_h cell)
{
    if (array->_alloctype == ALLOC_TYPE_1_BLOCK)
        return &(((void **)array->_map)[(cell->_xyz._x) +
                                        (cell->_xyz._y * array->_len) +
                                        (cell->_xyz._z * array->_len * array->_len)]);
    else if (array->_alloctype == ALLOC_TYPE_M_BLOCK)
        return &(((void ****)array->_map)[cell->_xyz._z][cell->_xyz._y][cell->_xyz._x]);
    else
        assert(false);
}

void **dyn2dget(dyn3darr_h array, coord_t *cell)
{
    if (array->_alloctype == ALLOC_TYPE_1_BLOCK)
        return &((void **)array->_map)[cell->_x + cell->_y * array->_len];
    else if (array->_alloctype == ALLOC_TYPE_M_BLOCK)
        return &((void ***)array->_map)[cell->_y][cell->_x];
    else
        assert(false);
}
