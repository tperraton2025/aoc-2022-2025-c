#include "partx.h"

void freeregion(void *arg)
{
    dll_free_all((dll_head_h)arg, free);
    free(arg);
}

DLL_NODE_CTOR(distnode_t, distnode_ctor);

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c);

int parse3dcoordinates(void *arg, char *str)
{
    context_h ctx = (context_h)arg;
    if (!ctx->lowest)
    {
        ctx->lowest = float3Dnode_ctor(float3D_ctor(0.0f, 0.0f, 0.0f));
        ctx->highest = float3Dnode_ctor(float3D_ctor(0.0f, 0.0f, 0.0f));
    }
    float sx, sy, sz;
    if (3 == sscanf(str, "%f,%f,%f", &sx, &sy, &sz))
    {
        float3Dnode_h n3d = float3Dnode_ctor(float3D_ctor(sx, sy, sz));
        dll_node_append(&ctx->_rawjboxlist, &n3d->_node);
        ctx->highest->_data._x = HIGHEST(ctx->highest->_data._x, sx);
        ctx->highest->_data._y = HIGHEST(ctx->highest->_data._y, sy);
        ctx->highest->_data._z = HIGHEST(ctx->highest->_data._x, sz);
        ctx->lowest->_data._x = LOWEST(ctx->lowest->_data._x, sx);
        ctx->lowest->_data._y = LOWEST(ctx->lowest->_data._y, sy);
        ctx->lowest->_data._z = LOWEST(ctx->lowest->_data._z, sz);
        return 0;
    }
    return EINVAL;
}

dll_node_h is_shorter_dist(dll_node_h _a, dll_node_h _b)
{
    distnode_h _dista = DIST_H(_a);
    distnode_h _distb = DIST_H(_b);
    return _dista->_val < _distb->_val ? _a : _b;
}

void sortdistances(dll_head_h head, dll_head_h sorteddist)
{
    LL_FOREACH_P(distn, head)
    {
        distnode_h refndist = (distnode_h)distn;
        distnode_h ndist = distnode_ctor();
        ndist->_pointa = refndist->_pointa;
        ndist->_pointb = refndist->_pointb;
        ndist->_val = refndist->_val;
        dll_node_sorted_insert(sorteddist, &ndist->_node, is_shorter_dist);
    }
}

parser_t float3Dparser = {._name = "3d coord parser",
                          ._func = parse3dcoordinates};

float p3ddist(float3D_h _a, float3D_h _b)
{
    float ret = sqrtf(SQUARE(_a->_x - _b->_x) + SQUARE(_a->_y - _b->_y) + SQUARE(_a->_z - _b->_z));
    return ret;
}

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c)
{
    float3D_h A = &((float3Dnode_h)_a)->_data;
    float3D_h B = &((float3Dnode_h)_b)->_data;
    float3D_h C = &((float3Dnode_h)_c)->_data;
    return p3ddist(B, A) < p3ddist(B, C) ? _b : _c;
}

#define STR_BUFF_LEN(_str) (sizeof(_str))
#define STR_BUFFER(name, default) static char name[STR_BUFF_LEN(default)] = default;

STR_BUFFER(_strp3d, "[00000000:00000000:00000000] ");
const char *const strfloat3d(float3D_h p3d_h)
{
    snprintf(_strp3d, sizeof(_strp3d) - 1, "[%5.0f,%5.0f,%5.0f]", p3d_h->_x, p3d_h->_y, p3d_h->_z);
    return _strp3d;
}

void printplist(dll_head_h head, const char *const fmt)
{
    LL_FOREACH_P(_pointn, head)
    {
        float3D_h p3d_h = &((float3Dnode_h)_pointn)->_data;
        printf("%s\n", strfloat3d(p3d_h));
    }
}

STR_BUFFER(_strdist, "[00000000,00000000,00000000] <--> [00000000,00000000,00000000] = 000000000000000")
const char *const strdist(distnode_h _dist, const char *const fmt)
{
    char *pstr = _strdist;
    pstr += snprintf(pstr, ANSI_MAX_LEN, "%s", fmt);
    pstr += sprintf(pstr, "%s", strfloat3d(_dist->_pointa));
    pstr += snprintf(pstr, sizeof(" <--> "), " <--> ");
    pstr += sprintf(pstr, "%s", strfloat3d(_dist->_pointb));
    pstr += snprintf(pstr, 10, " = %.0f", _dist->_val);
    assert(pstr - _strdist < sizeof(_strdist));
    return _strdist;
}

size_t printdlist(dll_head_h head, const char *const fmt)
{
    size_t missingdist = 0;
    LL_FOREACH_P(distn, head)
    {
        distnode_h disth = (distnode_h)distn;
        float3D_h p3d_h = disth->_pointa;
        if (!disth->_pointb)
            missingdist++;
        p3d_h = disth->_pointb;
        if (p3d_h)
        {
            printf("%s%s", fmt, strdist(disth, ""));
        }
        printf("\n");
    }
    printf(RESET "\n");
    return missingdist;
}

size_t missingmeasures(dll_head_h head)
{
    size_t missingdist = 0;
    LL_FOREACH_P(distn, head)
    {
        distnode_h disth = (distnode_h)distn;
        float3D_h p3d_h = disth->_pointa;
        if (!disth->_pointb)
            missingdist++;
        p3d_h = disth->_pointb;
    }
    return missingdist;
}

uint3D_t selectregion(context_h ctx, float3D_h np3d)
{
    if (!ctx->_regionssize._xyz._x)
    {
        ctx->_regionssize._xyz._x = (ctx->highest->_data._x - ctx->lowest->_data._x) / (1.0f * ctx->_segments);
        ctx->_regionssize._xyz._y = (ctx->highest->_data._y - ctx->lowest->_data._y) / (1.0f * ctx->_segments);
        ctx->_regionssize._xyz._z = (ctx->highest->_data._z - ctx->lowest->_data._z) / (1.0f * ctx->_segments);
    }

    uint3D_t ret = {0};
    for (size_t _regind = 0; _regind < ctx->_segments - 1; _regind++)
    {
        if (np3d->_x > _regind * ctx->_regionssize._xyz._x)
            ret._xyz._x++;
        if (np3d->_y > _regind * ctx->_regionssize._xyz._y)
            ret._xyz._y++;
        if (np3d->_z > _regind * ctx->_regionssize._xyz._z)
            ret._xyz._z++;
    }
    assert(ret._xyz._x <= ctx->_segments);
    assert(ret._xyz._y <= ctx->_segments);
    assert(ret._xyz._z <= ctx->_segments);
    return ret;
}

void dispatchpoints(context_h ctx)
{

    LL_FOREACH(_pn, ctx->_rawjboxlist)
    {
        float3D_h np3d = (float3D_h) & ((float3Dnode_h)_pn)->_data;
        distnode_h ndist = distnode_ctor();
        ndist->_pointa = np3d;
        ndist->_val = 9000000000000.0f;

        uint3D_t selreg = selectregion(ctx, np3d);
        dll_head_h *region = getregion(ctx, &selreg);
        if (!*region)
        {
            *region = (dll_head_h)malloc(sizeof(dll_head_t));
            dll_head_init(*region);
        }
        size_t size = sizeof(distnode_t);
        dll_node_append(*region, &ndist->_node);
    }
}

void keepclosestpoints(dll_head_h regiona, dll_head_h regionb)
{
    LL_FOREACH_P(_pna, regiona)
    {
        distnode_h dista = (distnode_h)_pna;
        LL_FOREACH_P(_pnb, regionb)
        {
            /** trimming out when a points distance to itself is measured  */
            if (_pna == _pnb)
                continue;
            distnode_h distb = (distnode_h)_pnb;
            float ndist = p3ddist(dista->_pointa, distb->_pointa);

            if (dista->_val > ndist)
            {
                dista->_val = ndist;
                distb->_val = ndist;
                dista->_pointb = distb->_pointa;
                distb->_pointb = dista->_pointa;
            }
        }
    }
}

STR_BUFFER(_struint3D, "[000:000:000] ");
const char *const struint3D(const uint3D_t *const a)
{
    snprintf(_struint3D, sizeof(_struint3D) - 1, "[%lu,%lu,%lu]", a->_xyz._x, a->_xyz._y, a->_xyz._z);
    return _struint3D;
}

void measuredistances(context_h ctx)
{
    bool atleastone = false;

    /** making a smaller selection of points for a manageable iteration time */

    size_t scanned = 0;

    RANGE_FOR(ind, 0, ctx->_segments + 2)
    {
        uint3D_t scanall = {0};

        /**
         * focusing on all regions of same z coordinate.
         *  in the region mapping, an incrementation of the z coordinate
         *  happens when the (x,y) combinations were all used
         */
        scanall._xyz._z = ind;
        printf(BGREEN "scanning from %s", struint3D(&scanall));

        dll_head_h *frstregiona_p = getregion(ctx, &scanall);
        dll_head_h *frstregionb_p = getregion(ctx, &scanall);

        scanall._xyz._z++;

        printf(" until %s\n" RESET, struint3D(&scanall));

        assert(scanall._xyz._z <= ctx->_segments + 2);
        dll_head_h *lastregiona_p = getregion(ctx, &scanall);
        dll_head_h *lastregionb_p = getregion(ctx, &scanall);

        scanall._xyz._z--;

        while (frstregiona_p < lastregiona_p)
        {
            while (frstregionb_p < lastregionb_p)
            {
                if (*frstregiona_p)
                {
                    /* aoc_info("region a %lu has points ",
                             (*frstregiona_p)->_size);
                     */
                    scanned += (*frstregiona_p)->_size;
                }
                if (*frstregiona_p && *frstregionb_p)
                {
                    keepclosestpoints(*frstregiona_p, *frstregionb_p);
                }
                frstregionb_p += sizeof(void *);
            }
            frstregionb_p = getregion(ctx, &scanall);
            frstregiona_p += sizeof(void *);
        }
        aoc_info("region pointers scanned %lu points ", scanned);
    }
}

size_t printregions(context_h ctx)
{
    uint3D_t scanall = {._arr = {0LU, 0LU, 0LU}};

    size_t distances = 0;
    size_t notmeasured = 0;

    size_t totalregioncount = 0;
    size_t emptyregioncount = 0;
    size_t validregioncount = 0;

    do
    {
        dll_head_h *region = (getregion(ctx, &scanall));
        if (!*region)
        {
            emptyregioncount++;
            continue;
        }
        validregioncount++;
        distances += (*region)->_size;
        // aoc_info("%s region has %lu has points ", struint3D(&scanall), (*region)->_size);

        size_t missing = missingmeasures(*region);
        // if (missing)
        //     aoc_err("%lu distances not measured", missing);
        notmeasured += missing;

        sortdistances(*region, &ctx->_distances);

    } while (!scanuint_array(scanall._arr, 0, ctx->_segments, ARRAY_DIM(scanall._arr)));
    totalregioncount = emptyregioncount + validregioncount;

    assert(distances == 20LU || distances == 1000LU);
    assert(((ctx->_segments + 1) * (ctx->_segments + 1) * (ctx->_segments + 1)) == totalregioncount);

    printf(BGREEN "there are a total of %lu/%lu occupied regions, for a total of %lu points\n" RESET,
           validregioncount, totalregioncount,
           distances);
    printf(YELLOW "%lu regions are empty\n" RESET,
           emptyregioncount);

    printdlist(&ctx->_distances, BGREEN);
    return notmeasured;
}

void getcircuitsizes(context_h ctx)
{
    dll_node_t ghostnode;
    dll_head_t circuitsizes;

    LL_FOREACH(node, ctx->_distances)
    {
        size_t circuitsize = 1;
        distnode_h distn = (distnode_h)node;
    }
}