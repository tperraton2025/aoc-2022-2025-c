#include "partx.h"

DLL_NODE_CTOR(float3Dn_h_t, float3Dn_ctor);
DLL_NODE_CTOR(distnode_t, floatnode_ctor);

float3D_t float3D_ctor(float x, float y, float z)
{
    float3D_t val = {._x = x, ._y = y, ._z = z};
    return val;
}

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c);

int parse3dcoordinates(void *arg, char *str)
{
    context_h ctx = (context_h)arg;
    float3D_t lowest = float3D_ctor(0.0f, 0.0f, 0.0f);
    float3D_t highest = float3D_ctor(0.0f, 0.0f, 0.0f);
    float sx, sy, sz;
    if (3 == sscanf(str, "%f,%f,%f", &sx, &sy, &sz))
    {
        float3Dn_h n3d = float3Dn_ctor();
        n3d->_data = float3D_ctor(sx, sy, sz);
        dll_node_append(&ctx->_rawjboxlist, &n3d->_node);
        if (lowest._x > sx)
            lowest._x = sx;
        if (lowest._y > sy)
            lowest._y = sy;
        if (lowest._x > sz)
            lowest._x = sz;
        if (highest._x < sx)
            highest._x = sx;
        if (highest._y < sy)
            highest._y = sy;
        if (highest._x < sz)
            highest._x = sz;
        return 0;
    }
    return EINVAL;
}

dll_node_h is_shorter_dist(dll_node_h _a, dll_node_h _b)
{
    distnode_h _dista = DIST_H(_a);
    distnode_h _distb = DIST_H(_b);
    return _dista->_dist < _distb->_dist ? _a : _b;
}

int sortdistances(dll_head_h head, dll_head_h dists)
{
    LL_FOREACH_P(_pointa, head)
    {
        LL_FOREACH_P(_pointb, head)
        {
            if (_pointa != _pointb)
            {
                distnode_h ndist = floatnode_ctor();
                ndist->_pointa = *P3D_H(_pointa);
                ndist->_pointb = *P3D_H(_pointb);
                ndist->_dist = p3ddist(&ndist->_pointa,
                                       &ndist->_pointb);

                dll_node_sorted_insert(dists, &ndist->_node, is_shorter_dist);
            }
        }
    }
}

parser_t float3Dparser = {._name = "3d coord parser",
                          ._func = parse3dcoordinates};

float p3ddist(float3D_h _a, float3D_h _b)
{
    float ret = sqrtf(SQUARE(_a->_x - _b->_x) + SQUARE(_a->_y - _b->_y) + SQUARE(_a->_z - _b->_z));
    return ret;
}

static char _str3ddist[STR_LEN] = "000.000 ";
const char *const strdist(float3D_h _a, float3D_h _b)
{
    float _dist = p3ddist(_a, _b);
    snprintf(_str3ddist, STR_LEN - 1, "%3.3f", _dist);
    return _str3ddist;
}

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c)
{
    float3D_h A = &((float3Dn_h)_a)->_data;
    float3D_h B = &((float3Dn_h)_b)->_data;
    float3D_h C = &((float3Dn_h)_c)->_data;
    return p3ddist(B, A) < p3ddist(B, C) ? _b : _c;
}

static char _strp3d[STR_LEN] = "000 : 000: 000 ";
const char *const strp3D(float3D_h p3d_h)
{
    snprintf(_strp3d, STR_LEN - 1, "%3.0f ; %3.0f ; %3.0f", p3d_h->_x, p3d_h->_y, p3d_h->_z);
    return _strp3d;
}

const char *const printplist(dll_head_h head, const char *const fmt)
{
    char _strp3da[STR_LEN] = "000 : 000: 000 ";
    char _strp3db[STR_LEN] = "000 : 000: 000 ";
    char *_strp3dnull = " ";

    LL_FOREACH_P(_pointn, head)
    {
        float3D_h _p3d = &((float3Dn_h)_pointn)->_data;
        float3D_h _np3d = NULL;

        strncpy(_strp3da, strp3D(_p3d), STR_LEN - 1);
        if (_pointn->_next)
        {
            _np3d = &((float3Dn_h)_pointn->_next)->_data;
            strncpy(_strp3db, strp3D(_np3d), STR_LEN - 1);
        }

        if (_np3d)
        {
            printf("%s%s\t\t%s -> %s\n", fmt, _strp3da, _strp3db, strdist(_p3d, _np3d));
        }
        else
            printf("%s%s \n", fmt, _strp3da);
    }
}

void printdlist(dll_head_h head, const char *const fmt)
{
    char _strp3da[STR_LEN] = "000 : 000: 000 ";
    char _strp3db[STR_LEN] = "000 : 000: 000 ";

    LL_FOREACH_P(_node, head)
    {
        distnode_h _dist = (distnode_h)_node;
        strncpy(_strp3da, strp3D(&_dist->_pointa), STR_LEN - 1);
        strncpy(_strp3db, strp3D(&_dist->_pointb), STR_LEN - 1);

        printf("%s%s\t\t%s -> %.0f\n", fmt, _strp3da, _strp3db, _dist->_dist);
    }
}

uint3D_t getregion(context_h ctx, float3D_h np3d)
{
    if (!ctx->_regionssize._x)
    {
        ctx->_regionssize._x = ctx->highest._x - ctx->lowest._x;
        ctx->_regionssize._y = ctx->highest._y - ctx->lowest._y;
        ctx->_regionssize._z = ctx->highest._z - ctx->lowest._z;
    }

    uint3D_t ret = {0};
    ret._x = ((ctx->_regionssize._x / np3d->_x) * (1.0f * ctx->_segments));
    ret._y = ((ctx->_regionssize._y / np3d->_y) * (1.0f * ctx->_segments));
    ret._z = ((ctx->_regionssize._z / np3d->_z) * (1.0f * ctx->_segments));
    assert(ret._x < ctx->_segments);
    assert(ret._y < ctx->_segments);
    assert(ret._z < ctx->_segments);
    return ret;
}

void dispatchpoints(context_h ctx)
{

    LL_FOREACH(_pn, ctx->_rawjboxlist)
    {
        float3D_h np3d = (float3D_h) & ((float3Dn_h)_pn)->_data;
        distnode_h ndist = distnode_ctor();
        ndist->_pointa = np3d;
        ndist->_pointb = NULL;
        ndist->_dist = 999999999999999999.0f;

        uint3D_t region = getregion(ctx, np3d);

        dll_node_append(ctx->_regions[region._x][region._y][region._z], &ndist->_node);
    }
}

void keepclosestpoint(dll_head_h regiona, dll_head_h regionb)
{
    LL_FOREACH_P(_pna, regiona)
    {
        distnode_h dista = (distnode_h)_pna;
        LL_FOREACH_P(_pnb, regionb)
        {
            distnode_h distb = (distnode_h)_pnb;
            float ndist = p3ddist(dista->_pointa, distb->_pointa);
            if (dista->_dist > ndist)
            {
                dista->_dist = ndist;
                distb->_dist = ndist;
                dista->_pointb = distb->_pointa;
                distb->_pointb = dista->_pointa;
            }
        }
    }
}

void measuredistances(context_h ctx, uint3D_h coord)
{
    for (size_t _dx = 1; _dx < 3; _dx++)
    {
        for (size_t _dy = 1; _dy < 3; _dy++)
        {
            for (size_t _dz = 1; _dz < 3; _dz++)
            {

                LL_FOREACH_P(_regiona, ctx->_regions[coord->_x][coord->_y][coord->_z])
                {
                    if ((coord->_x + _dx < ctx->_segments) &&
                        (coord->_y + _dy < ctx->_segments) &&
                        (coord->_z + _dz < ctx->_segments))
                        keepclosestpoint(_regiona,
                                         ctx->_regions[coord->_x + _dx][coord->_y + _dy][coord->_z + _dz]);
                }
            }
        }
    }
}