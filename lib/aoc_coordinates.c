
#include "aoc_coordinates.h"
#include "aoc_ranges.h"

uint3D_t uint3d_ctor(size_t x, size_t y, size_t z);

uint3D_t uint3d_sum(const uint3D_t *const a, const uint3D_t *const b)
{
    uint3D_t sum = {0};
    ARR_FOREACH(ind, a->_arr)
    {
        sum._arr[ind] = a->_arr[ind] + b->_arr[ind];
    }
    return sum;
}

bool uint3d_eq(const uint3D_t *const a, const uint3D_t *const b)
{
    return !memcmp(a->_arr, b->_arr, 3LU);
}

STR_BUFFER(_struint3D, "[000:000:000] ");
const char *const struint3D(const uint3D_t *const a)
{
    snprintf(_struint3D, sizeof(_struint3D) - 1, "[%lu,%lu,%lu]", a->_xyz._x, a->_xyz._y, a->_xyz._z);
    return _struint3D;
}

int expuint3d(uint3D_t *array, uint3D_t *max)
{
    size_t *_max = max->_arr;
    size_t *_arr = array->_arr;
    bool carryover = false;
    int ret = 0;
    bool couldinc = false;
    RANGE_FOR(ind, 0LU, 3LU)
    {
        if (_arr[ind] < _max[ind])
        {
            _arr[ind]++;
            couldinc = true;
        }
    }
    return couldinc ? 0 : EOVERFLOW;
}

int shruint3d(uint3D_t *array, uint3D_t *min)
{
    size_t *_min = min->_arr;
    size_t *_arr = array->_arr;

    bool carryover = false;
    int ret = 0;
    bool coulddec = false;
    RANGE_FOR(ind, 0LU, 3LU)
    {
        if (_arr[ind] > _min[ind])
        {
            _arr[ind]--;
            coulddec = true;
        }
    }
    return coulddec ? 0 : ERANGE;
}

int bscanuint3d(uint3D_t *array, uint3D_t *min, uint3D_t *max)
{
    size_t *_array = array->_arr;
    size_t *_min = min->_arr;
    size_t *_max = max->_arr;

    for (size_t ind = 3LU - 1; ind < __SIZE_MAX__; ind--)
    {
        if (_min[ind] <= _max[ind])
            break;
        assert(_min[ind] <= _max[ind]);
    }

    bool carryover = false;
    int ret = 0;
    RANGE_FOR(ind, 0LU, 3LU)
    {
        _array[ind]++;
        if (_array[ind] > _max[ind])
        {
            _array[ind] = _min[ind];
            if (ind == 3LU - 1)
            {
                RANGE_FOR(clearind, 0LU, 3LU)
                {
                    _array[clearind] = _min[clearind];
                }
                return EOVERFLOW;
            }
            continue;
        }
        break;
    }
    /**
     * debuging
     * RANGE_FOR(ind, 0LU, 3LU)
     * {
     *     printf("%lu ", array[ind]);
     * }
     * printf("\n");
     * */
    return ret;
}

int scanuint(uint3D_t *array, size_t max, size_t dim)
{
    size_t *_array = array->_arr;

    bool carryover = false;
    int ret = 0;
    RANGE_FOR(ind, 0LU, dim)
    {
        assert(_array[ind] < max + 1);
        _array[ind]++;
        if (_array[ind] > max)
        {
            _array[ind] = 0LU;
            if (ind == dim - 1)
            {
                RANGE_FOR(clearind, 0LU, dim)
                {
                    _array[clearind] = 0LU;
                }

                return EOVERFLOW;
            }
            continue;
        }
        break;
    }
    /**
     * debuging
     * RANGE_FOR(ind, 0LU, 3LU)
     * {
     *     printf("%lu ", array[ind]);
     * }
     * printf("\n");
     * */
    return ret;
}

bool coord_dist_more_than_2(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    size_t disty = ABSDIFF(_acord->_y, _bcord->_y);
    return distx > 2 && disty > 2;
}

bool coord_dist_less_than_2(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    size_t disty = ABSDIFF(_acord->_y, _bcord->_y);
    return distx < 2 && disty < 2;
}

bool coord_on_adjacent_columns(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    return distx < 2;
}

bool coord_higher_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return _bcord->_y > _acord->_y;
}

bool coord_lower_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return _bcord->_y < _acord->_y;
}

bool coord_x_equal(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return _acord->_x == _bcord->_x;
}

/** return true if _b point has higher Y on same X */
bool coord_equal_x_higher_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;
    return ((_bcord->_y > _acord->_y) && (_acord->_x == _bcord->_x));
}

/** return true if _b point has higher Y on same X */
bool coord_equal_x_lower_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coord_tracker_h, _a)->_coord;
    coord_t *_bcord = &CAST(coord_tracker_h, _b)->_coord;

    return ((_bcord->_y < _acord->_y) && (_bcord->_x == _acord->_x));
}

bool coord_same_column(void *arga, void *argb)
{
    coord_t *_pos = (coord_t *)argb;
    return _pos->_x == (size_t)arga;
}

dll_node_h highest_column(dll_node_h arga, dll_node_h argb)
{
    coord_t *_posa = (coord_t *)arga;
    coord_t *_posb = (coord_t *)argb;
    return (dll_node_h)(_posa->_x > _posb->_x ? _posa : _posb);
} 

dll_node_h coordtrackernode_ctor(coord_t *coord)
{
    coord_tracker_h new = NULL; 
    TRY_TYPE_MALLOC(new, coord_tracker_t);
    new->_coord._x = coord->_x;
    new->_coord._y = coord->_y;
    return &new->_node;
}

static char _strpos[] = "[" STR(__SIZE_MAX__) ":" STR(__SIZE_MAX__) "]";
/*! warning: not thread safe */
char *strpos(const coord_t *const pos)
{
    snprintf(_strpos, ARRAY_DIM(_strpos), "[%.1lu:%.1lu]", pos->_x, pos->_y); 
    return _strpos;
}
