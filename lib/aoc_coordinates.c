
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

int bscanuint2d(uint2D_t *array, uint2D_t *min, uint2D_t *max)
{
    size_t *_array = array->_arr;
    size_t *_min = min->_arr;
    size_t *_max = max->_arr;

    for (size_t ind = 1LU; ind < __SIZE_MAX__; ind--)
    {
        if (_min[ind] <= _max[ind])
            break;
        assert(_min[ind] <= _max[ind]);
    }

    bool carryover = false;
    int ret = 0;
    RANGE_FOR(ind, 0LU, 2LU)
    {
        _array[ind]++;
        if (_array[ind] > _max[ind])
        {
            _array[ind] = _min[ind];
            if (ind == 1LU)
            {
                RANGE_FOR(clearind, 0LU, 2LU)
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

    RANGE_FOR(ind, 0LU, 2LU)
    {
        printf("%lu ", array->_arr[ind]);
    }
    printf("\n");

    /* */
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
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    size_t disty = ABSDIFF(_acord->_y, _bcord->_y);
    return distx > 2 && disty > 2;
}

bool coord_dist_less_than_2(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    size_t disty = ABSDIFF(_acord->_y, _bcord->_y);
    return distx < 2 && disty < 2;
}

bool coord_on_adjacent_columns(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    return distx < 2;
}

bool coord_higher_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    return _bcord->_y > _acord->_y;
}

bool coord_lower_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    return _bcord->_y < _acord->_y;
}

bool coord_x_equal(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    return _acord->_x == _bcord->_x;
}

/** return true if _b point has higher Y on same X */
bool coordnodes_equal_x_higher_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;
    return ((_bcord->_y > _acord->_y) && (_acord->_x == _bcord->_x));
}

/** return true if _b point has higher Y on same X */
bool coordnodes_equal_x_lower_y(void *_a, void *_b)
{
    coord_t *_acord = &CAST(coordnode_h, _a)->_coord;
    coord_t *_bcord = &CAST(coordnode_h, _b)->_coord;

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

/** return true if _b point has higher Y on same X */
dll_node_h coordnodes_equal_y_lower_x(dll_node_h arga, dll_node_h argb)
{
    coord_t *_posa = (coord_t *)arga;
    coord_t *_posb = (coord_t *)argb;

    if ((_posa->_y != _posb->_y))
        return argb;
    return _posa->_x < _posb->_x ? arga : argb;
}

dll_node_h coord_y_aligned(dll_node_h arga, dll_node_h argb)
{
    coord_t *posa = (coord_t *)arga;
    coord_t *posb = (coord_t *)argb;
    if (posa->_x == posb->_x)
        return posa->_y > posb->_y ? arga : argb;
    return posa->_x > posb->_x ? arga : argb;
}

dll_node_h coordnode_ctor(coord_t *coord)
{
    coordnode_h new = NULL;
    new = calloc(1LU, sizeof(coordnode_t));
    new->_coord._x = coord->_x;
    new->_coord._y = coord->_y;
    return &new->_node;
}

bool coordt_equal(void *arga, void *argb)
{
    coord_t *posa = (coord_t *)arga;
    coord_t *posb = (coord_t *)argb;
    return (posa->_x == posb->_x) && (posa->_y == posb->_y);
}

dll_node_h coordpairnode_ctor(coordpair_h pair)
{
    coordpairnode_h new = NULL;
    new = calloc(1LU, sizeof(coordpairnode_t));
    new->_pair._max = pair->_max;
    new->_pair._min = pair->_min;
    return &new->_node;
}

dll_node_h coordpair_equal_y_lower_x(dll_node_h _a, dll_node_h _b)
{
    coordpairnode_h apair = ((coordpairnode_h)_a);
    coordpairnode_h bpair = ((coordpairnode_h)_b);

    if (apair->_pair._max._y == bpair->_pair._max._y)
        return apair->_pair._max._x < bpair->_pair._max._x ? _a : _b;

    if (apair->_pair._max._y == bpair->_pair._min._y)
        return apair->_pair._max._x < bpair->_pair._min._x ? _a : _b;
    return _b;
}

/** return true if _b point has higher Y on same X */
dll_node_h coordnode_equal_x_higher_y(dll_node_h _a, dll_node_h _b)
{
    coord_t *apos = &((coordnode_h)_a)->_coord;
    coord_t *bpos = &((coordnode_h)_b)->_coord;

    if (apos->_x == bpos->_x)
        return apos->_y > bpos->_y ? _a : _b;
    return apos->_x < bpos->_x ? _a : _b;
}

dll_node_h coordpair_equal_y_higher_x(dll_node_h _a, dll_node_h _b)
{
    coordpairnode_h apair = ((coordpairnode_h)_a);
    coordpairnode_h bpair = ((coordpairnode_h)_b);

    if (apair->_pair._max._y == bpair->_pair._max._y)
        return apair->_pair._max._x > bpair->_pair._max._x ? _a : _b;

    if (apair->_pair._max._y == bpair->_pair._min._y)
        return apair->_pair._max._x > bpair->_pair._min._x ? _a : _b;
    return _b;
}

dll_node_h coordrefpairnode_ctor(coordpair_h pair)
{
    coordrefpairnode_h new = NULL;
    new = calloc(1LU, sizeof(coordrefpairnode_t));
    new->_first = &pair->_max;
    new->_second = &pair->_min;
    return &new->_node;
}

static char _strpos[] = "[" STR(__SIZE_MAX__) ":" STR(__SIZE_MAX__) "]";
/*! warning: not thread safe */
char *strpos(const coord_t *const pos)
{
    snprintf(_strpos, ARRAY_DIM(_strpos), "[%.1lu:%.1lu]", pos->_x, pos->_y);
    return _strpos;
}

static char _strpospair[] = "[" STR(__SIZE_MAX__) ":" STR(__SIZE_MAX__) "],[" STR(__SIZE_MAX__) ":" STR(__SIZE_MAX__) "]";
/*! warning: not thread safe */
char *strpospair(const coordpair_h const pair)
{
    snprintf(_strpospair,
             ARRAY_DIM(_strpospair),
             "[%.1lu:%.1lu],[%.1lu:%.1lu]",
             pair->_max._x,
             pair->_max._y,
             pair->_min._x,
             pair->_min._y);
    return _strpospair;
}

bool coordnodes_equal(void *_a, void *_b)
{
    coordnode_h apos = CAST(coordnode_h, _a);
    coordnode_h bpos = CAST(coordnode_h, _b);
    return (apos->_coord._x == bpos->_coord._x) && (apos->_coord._y == bpos->_coord._y);
}
