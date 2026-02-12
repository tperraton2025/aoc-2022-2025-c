#include "partx.h"

void pointnode_free(void *arg)
{
    dll_free_all(&((pointnode_h)arg)->_connections, free);
    free(arg);
}

void freeregion(void *arg)
{
    dll_free_all((dll_head_h)arg, pointnode_free);
    free(arg);
}

typenode_ctor(pointref,
              new->_dist = dist;
              new->_first_point = pointa;
              new->_second_point = pointb, float dist, pointnode_h pointa, pointnode_h pointb);

dll_node_h pointnode_ctor(float3D_h p3dref)
{
    pointnode_h npointnode = NULL;
    npointnode = calloc(1LU, sizeof(pointnode_t));
    npointnode->_f3d = p3dref;
    npointnode->_shortestdist = 9000000000000.0f;
    return &npointnode->_node;
}

bool areonoftwopoints(void *arga, void *argb)
{
    return ((float3D_h *)argb)[0] == ((pointnode_h)((voidpnode_h)arga)->_ptr)->_f3d ||
           ((float3D_h *)argb)[1] == ((pointnode_h)((voidpnode_h)arga)->_ptr)->_f3d;
}

bool aresamepoints(void *arga, void *argb)
{
    return ((float3D_h)argb) == ((pointnode_h)((voidpnode_h)arga)->_ptr)->_f3d;
}

bool issamepairofpoints(dll_node_h arga, dll_node_h argb)
{
    return ((pointrefnode_h)arga)->_first_point == ((pointrefnode_h)argb)->_second_point &&
           ((pointrefnode_h)arga)->_second_point == ((pointrefnode_h)argb)->_first_point;
}

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c);

int parse3dcoordinates(void *arg, char *str)
{
    context_h ctx = (context_h)arg;
    if (!ctx->_lowest)
    {
        ctx->_lowest = (float3Dnode_h)float3Dnode_ctor(float3D_ctor(0.0f, 0.0f, 0.0f));
        ctx->_highest = (float3Dnode_h)float3Dnode_ctor(float3D_ctor(0.0f, 0.0f, 0.0f));
    }
    float sx, sy, sz;
    if (3 == sscanf(str, "%f,%f,%f", &sx, &sy, &sz))
    {
        dll_node_append(&ctx->_inputpoints, float3Dnode_ctor(float3D_ctor(sx, sy, sz)));
        ctx->_highest->_data._x = HIGHEST(ctx->_highest->_data._x, sx);
        ctx->_highest->_data._y = HIGHEST(ctx->_highest->_data._y, sy);
        ctx->_highest->_data._z = HIGHEST(ctx->_highest->_data._x, sz);
        ctx->_lowest->_data._x = LOWEST(ctx->_lowest->_data._x, sx);
        ctx->_lowest->_data._y = LOWEST(ctx->_lowest->_data._y, sy);
        ctx->_lowest->_data._z = LOWEST(ctx->_lowest->_data._z, sz);
        return 0;
    }
    return EINVAL;
}

dll_node_h shorterdistpointref(dll_node_h _a, dll_node_h _b)
{
    pointrefnode_h _dista = ((pointrefnode_h)_a);
    pointrefnode_h _distb = ((pointrefnode_h)_b);
    return _dista->_dist < _distb->_dist ? _a : _b;
}

dll_node_h shorterdistvoidpointref(dll_node_h _a, dll_node_h _b)
{
    pointrefnode_h _dista = (pointrefnode_h)((voidpnode_h)_a)->_ptr;
    pointrefnode_h _distb = (pointrefnode_h)((voidpnode_h)_b)->_ptr;
    return _dista->_dist < _distb->_dist ? _a : _b;
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

STR_BUFFER(_strp3d, "       [00000000:00000000:00000000]    ");
const char *const strfloat3d(float3D_h p3d_h, const char *const fmt)
{
    snprintf(_strp3d, sizeof(_strp3d) - 1, "%s[%5.0f,%5.0f,%5.0f]" RESET, fmt, p3d_h->_x, p3d_h->_y, p3d_h->_z);
    return _strp3d;
}

void printpreflist(dll_head_h distances)
{
    size_t printlen = 0;
    LL_FOREACH_P(node, distances)
    {
        pointrefnode_h pointref = (pointrefnode_h)node;
        pointnode_h pnode_h = (pointnode_h)(pointref->_first_point);

        printf("%s ", strfloat3d(pnode_h->_f3d, GREEN));
        if (++printlen > 5)
        {
            printf(" ... ");
            break;
        }
    }
}

void printdlist(dll_head_h distances)
{
    size_t printlen = 0;
    LL_FOREACH_P(node, distances)
    {
        pointrefnode_h pointref = (pointrefnode_h)node;
        pointnode_h pnode_h = (pointnode_h)(pointref->_first_point);

        printf("%5.0f ", pnode_h->_shortestdist);
        if (++printlen > 5)
        {
            printf(" ... ");
            break;
        }
    }
}

void printplist(dll_head_h points)
{
    size_t printlen = 0;
    LL_FOREACH_P(node, points)
    {
        pointnode_h pnode_h = (pointnode_h)node;

        printf("%s ", strfloat3d(pnode_h->_f3d, GREEN));
        if (++printlen > 5)
        {
            printf(" ... ");
            break;
        }
    }
}

size_t printdistancesegs(context_h ctx, bool verbose)
{
    size_t sorteddistances = 0;
    size_t printlen = 0;
    ARR_FOREACH(ind, ctx->_distances)
    {
        if (ctx->_distances[ind]._size && verbose)
        {
            printf("\n");
            printf("-- distance segment %5lu: %5lu ", ind, ctx->_distances[ind]._size);
            printdlist(&ctx->_distances[ind]);
        }
        sorteddistances += ctx->_distances[ind]._size;
    }
    if (verbose)
        printf("\n");
    return sorteddistances;
}

STR_BUFFER(_strpoint, "[00000000,00000000,00000000] <--> [00000000,00000000,00000000] = 000000000000000")
const char *const strpoint(pointnode_h _dist, const char *const fmt)
{
    char *pstr = _strpoint;
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), "%s", fmt);
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), "%s", strfloat3d(_dist->_f3d, ""));
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), " <--> ");
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), "%s", strfloat3d(_dist->_closest->_f3d, ""));
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), " = %.0f", _dist->_shortestdist);

    assert(pstr - _strpoint < sizeof(_strpoint));
    return _strpoint;
}

STR_BUFFER(_strpref, "[00000000,00000000,00000000] <--> [00000000,00000000,00000000] = 000000000000000")
const char *const strpref(pointrefnode_h _dist, const char *const fmt)
{
    char *pstr = _strpref;

    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), "%s", fmt);
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), "%s", strfloat3d(_dist->_first_point->_f3d, ""));
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), " <--> ");
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), "%s", strfloat3d(_dist->_second_point->_f3d, ""));
    pstr += snprintf(pstr, sizeof(_strpoint) - (pstr - _strpoint), " = %.0f", _dist->_dist);

    assert(pstr - _strpref < sizeof(_strpref));
    return _strpref;
}

size_t missingmeasures(context_h ctx)
{
    size_t missingdist = 0;
    uint3D_t scan = {0};
    do
    {
        dll_head_h region = &ctx->_regions[scan._xyz._x][scan._xyz._y][scan._xyz._z];
        LL_FOREACH_P(distn, region)
        {
            pointnode_h disth = (pointnode_h)distn;
            float3D_h p3d_h = disth->_f3d;
            if (!disth->_connections._size && !disth->_closest)
                missingdist++;
        }
    } while (!scanuint3d(&scan, RSIZE));
    return missingdist;
}

uint3D_t selectregion(context_h ctx, float3D_h np3d)
{
    uint3D_t ind = {0};

    ind._xyz._x = ((size_t)((np3d->_x / ctx->_highest->_data._x * (1.0f * RSIZE))));
    ind._xyz._y = ((size_t)((np3d->_y / ctx->_highest->_data._y * (1.0f * RSIZE))));
    ind._xyz._z = ((size_t)((np3d->_z / ctx->_highest->_data._z * (1.0f * RSIZE))));

    ind._xyz._x = ind._xyz._x ? ind._xyz._x - 1 : 0LU;
    ind._xyz._y = ind._xyz._y ? ind._xyz._y - 1 : 0LU;
    ind._xyz._z = ind._xyz._z ? ind._xyz._z - 1 : 0LU;

    assert(ind._xyz._x < RSIZE);
    assert(ind._xyz._y < RSIZE);
    assert(ind._xyz._z < RSIZE);
    return ind;
}

void dispatchpoints(context_h ctx)
{

    LL_FOREACH(_pn, ctx->_inputpoints)
    {
        float3D_h p3dref = (float3D_h) & ((float3Dnode_h)_pn)->_data;

        uint3D_t selreg = selectregion(ctx, p3dref);
        dll_node_append(&getregion(ctx, selreg), pointnode_ctor(p3dref));
    }
}

size_t sortpointsreferencesbydistance(context_h ctx, dll_head_h regiona, dll_head_h regionb, bool verbose)
{
    size_t distancesscanned = 0;

    if (verbose)
    {
        printf("-- scanning" GREEN " (%lu) " RESET, regiona->_size);
        printplist(regiona);
        printf("and " GREEN " (%lu) " RESET, regionb->_size);
        printplist(regionb);
        printf("\n");
    }

    LL_FOREACH_P(_pna, regiona)
    {
        pointnode_h pointa_h = (pointnode_h)_pna;
        LL_FOREACH_P(_pnb, regionb)
        {
            /**
             *
             * stopping when a points distance to itself is measured
             * this happens when regiona == regionb.
             * (which is necessary for regions with several points)
             * In order to prevent double measurement of distances,
             * between points of same regions, the search must stop
             * at the middle of the regiona point set.
             * when regiona != regionb, _pna == _pnb never happens
             * as regions are exclusive in points distribution.
             *
             * Although, this does not remove the cases of regions
             * alternatively searched as regiona and regionb,
             * as peekscubicsubregion cannot discriminate regions
             * previously polled as center or head.
             *
             * */

            if (_pna == _pnb)
                break;

            pointnode_h pointb_h = (pointnode_h)_pnb;

            float ndist = p3ddist(pointa_h->_f3d, pointb_h->_f3d);

            float distanceind = ndist / ctx->_longestdistance * (ARRAY_DIM(ctx->_distances) * 1.0f);
            size_t index = (size_t)(distanceind);
            assert(index < ARRAY_DIM(ctx->_distances));

            dll_node_h new = pointrefnode_ctor(ndist, pointa_h, pointb_h);
            int insert = dll_node_sorted_insert_if_absent(&ctx->_distances[index],
                                                          new,
                                                          shorterdistpointref, issamepairofpoints);
            if (insert)
                free(new);
            distancesscanned++;

            if (pointa_h->_shortestdist > ndist)
            {
                pointa_h->_shortestdist = ndist;
                pointa_h->_closest = pointb_h;
            }
            if (pointb_h->_shortestdist > ndist)
            {
                pointb_h->_shortestdist = ndist;
                pointb_h->_closest = pointa_h;
            }
        }
    }
    return distancesscanned;
}

void connectpoints(context_h ctx, size_t *connections, pointnode_h pointa_h, pointnode_h pointb_h, bool verbose)
{
    dll_node_sorted_insert(&pointa_h->_connections, voidpnode_ctor(pointb_h), shorterdistvoidpointref);
    dll_node_sorted_insert(&pointb_h->_connections, voidpnode_ctor(pointa_h), shorterdistvoidpointref);

    if (verbose)
    {
        printf("%4lu -- ", *connections);
        printf("%s %s (%lu)", "connected", strfloat3d(pointa_h->_f3d, GREEN), pointa_h->_connections._size);
        printf("\tand %s (%lu)\n", strfloat3d(pointb_h->_f3d, GREEN), pointb_h->_connections._size);
    }
    (*connections)++;
}

size_t connect_x_unconnectedpoints(context_h ctx, size_t x, bool verbose)
{
    size_t connections = 1LU;
    ARR_FOREACH(ind, ctx->_distances)
    {
        if (connections > x)
            break;
        LL_FOREACH(pointaref, ctx->_distances[ind])
        {
            pointrefnode_h pointref = (pointrefnode_h)pointaref;
            pointnode_h first = pointref->_first_point;
            pointnode_h second = pointref->_second_point;
            connectpoints(ctx, &connections, first, second, verbose);
            first->_connected = true;
            second->_connected = true;
            if (connections > x)
            {
                return ((size_t)(first->_f3d->_x * second->_f3d->_x));
            }
        }
    }
}

void connectalluntilonecircuit(context_h ctx, bool verbose)
{
    size_t connections = 1LU;
    ARR_FOREACH(ind, ctx->_distances)
    {
        LL_FOREACH(pointaref, ctx->_distances[ind])
        {
            pointrefnode_h pointref = (pointrefnode_h)pointaref;
            pointnode_h first = pointref->_first_point;
            pointnode_h second = pointref->_second_point;
            connectpoints(ctx, &connections, first, second, verbose);
            first->_connected = true;
            second->_connected = true;
        }
    }
}

void distancessearchedscan(context_h ctx, bool verbose)
{
    bool atleastone = false;

    /**
     * We are making a smaller selection of points for a manageable iteration time.
     *  Then scan for the shortest distance between each points in two consecutive
     *  regions. It scans twice each region on purpose to compare distances
     *  from every points with all the points in the previous and the next
     *  reion.
     */

    size_t distancesscanned = 0;

    ctx->_regions_origin._xyz._x = 0LU;
    ctx->_regions_origin._xyz._y = 0LU;
    ctx->_regions_origin._xyz._x = 0LU;

    ctx->_regions_end._xyz._x = RSIZE - 1;
    ctx->_regions_end._xyz._y = RSIZE - 1;
    ctx->_regions_end._xyz._z = RSIZE - 1;

    uint3D_t head = ctx->_regions_origin;
    int done = 0;

    do
    {
        while (!(getregion(ctx, head)._size) &&
               !done)
        {
            done = bscanuint3d(&head, &ctx->_regions_origin, &ctx->_regions_end);
        }
        peekcubicsubregion(ctx, &head, verbose);
        if (done)
            break;
        done = bscanuint3d(&head, &ctx->_regions_origin, &ctx->_regions_end);
    } while (!done);

    size_t sorteddistances = printdistancesegs(ctx, verbose);

    aoc_info("-- region pointers tested %lu shortest distances", sorteddistances);
}

size_t peekcubicsubregion(context_h ctx, uint3D_t *center, bool verbose)
{
    uint3D_t _cubemax = *center;
    uint3D_t _cubemin = *center;
    uint3D_t _head = _cubemin;
    uint3D_t _center = *center;
    size_t result = 0LU;
    bool found = false;

    while (!found)
    {
        int exp = expuint3d(&_cubemax, &ctx->_regions_end);
        int shr = shruint3d(&_cubemin, &ctx->_regions_origin);
        if (verbose)
        {
            printf(YELLOW "-- expanded %s ", struint3D(&_cubemin));
            printf(" %s \n" RESET, struint3D(&_cubemax));
        }
        if (exp && shr)
            return result;

        _head = _cubemin;
        do
        {
            if (uint3d_eq(&_head, &_center))
                continue;
            if (getregion(ctx, _head)._size)
            {
                found = true;
                break;
            }
        } while (!bscanuint3d(&_head, &_cubemin, &_cubemax));
    }

    _head = _cubemin;
    do
    {
        dll_head_h first_region = &getregion(ctx, _center);
        dll_head_h second_region = &getregion(ctx, _head);
        if (second_region->_size)
        {
            if (verbose)
            {
                printf("-- regions" GREEN " %s ", struint3D(&_center));
                printf(" %s \n" RESET, struint3D(&_head));
            }
            result += sortpointsreferencesbydistance(ctx,
                                                     first_region,
                                                     second_region, verbose);
        }
    } while (!bscanuint3d(&_head, &_cubemin, &_cubemax));

    return result;
}

size_t scanregions(context_h ctx, bool verbose)
{
    size_t distances = 0;
    size_t notmeasured = 0;

    size_t totalregioncount = 0;
    size_t emptyregioncount = 0;
    size_t validregioncount = 0;

    uint3D_t scan = ctx->_regions_origin;

    do
    {
        if (!(getregion(ctx, scan)._size))
        {
            emptyregioncount++;
            continue;
        }
        validregioncount++;
        distances += getregion(ctx, scan)._size;
        size_t missing = missingmeasures(ctx);
        notmeasured += missing;
    } while (!bscanuint3d(&scan, &ctx->_regions_origin, &ctx->_regions_end));

    if (verbose)
    {
        ARR_FOREACH(ind, ctx->_distances)
        {
            LL_FOREACH(node, ctx->_distances[ind])
            {
                pointrefnode_h pointref = (pointrefnode_h)node;
                printf("%s\n", strpref(pointref, ""));
            }
        }
    }

    totalregioncount = emptyregioncount + validregioncount;

    assert((distances == 20) || (distances == 1000));

    printf("-- there are a total of %lu/%lu occupied regions, for a total of %lu points\n",
           validregioncount, totalregioncount,
           distances);
    printf("-- %lu regions are empty\n",
           emptyregioncount);

    return notmeasured;
}

void getconnections(pointnode_h origin, pointnode_h current, pointnode_h from, dll_head_h circuit, bool verbose);

void getcircuitsize(pointnode_h start, pointnode_h origin, bool verbose)
{
    dll_head_t circuitsizes = {0};
    dll_head_t circuit = {0};

    if (verbose)
        printf("\n--start-- (%p)\n", start);

    getconnections(start, NULL, NULL, &circuit, verbose);
    if (verbose)
        printf("\t\t\t\t\t\t");
    if (verbose)
    {
        LL_FOREACH(pnode, circuit)
        {
            voidpnode_h vpnode = (voidpnode_h)pnode;
            float3D_h f3d = (float3D_h) & (((pointnode_h)(vpnode->_ptr))->_f3d);
            printf("%s ", strfloat3d(f3d, GREEN));
        }
        printf("\n");
    }
    dll_free_all(&circuit, free);
    dll_free_all(&circuitsizes, free);
}

void getconnections(pointnode_h origin, pointnode_h current, pointnode_h from, dll_head_h circuit, bool verbose)
{
    origin->_scanned = true;
    if (!from)
        from = origin;

    if (!current)
        current = origin;
    dll_node_append(circuit, voidpnode_ctor((void *)current));
    current->_scanned = true;

    if (verbose)
    {
        printf(BLUE "added %s (%p)\n", strfloat3d(current->_f3d, GREEN), current);
        printf(BLUE "entered %s\n", __func__);
        printf(BLUE "connections: %lu\n", current->_connections._size);
    }

    LL_FOREACH(pnode, current->_connections)
    {
        voidpnode_h pointh = (voidpnode_h)pnode;
        pointnode_h pnodeh = (pointnode_h)pointh->_ptr;

        if (pnodeh != origin && pnodeh != from && !pnodeh->_scanned)
        {
            getconnections(origin, pnodeh, current, circuit, verbose);
        }
    }
    current->_node._obsolete = true;
}

void reinitconnections(pointnode_h origin, pointnode_h current, pointnode_h from, dll_head_h circuit, bool verbose)
{
    origin->_scanned = false;
    if (!from)
        from = origin;

    if (!current)
        current = origin;
    current->_scanned = false;

    LL_FOREACH(pnode, current->_connections)
    {
        voidpnode_h pointh = (voidpnode_h)pnode;
        pointnode_h pnodeh = (pointnode_h)pointh->_ptr;

        if (pnodeh != origin && pnodeh != from && pnodeh->_scanned)
        {
            reinitconnections(origin, pnodeh, current, circuit, verbose);
        }
    }
}