#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_dynamic_array.h>
#include <aoc_numeric.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <float.h>

#define P3D_STRLEN (32)
#define DIST_STRLEN (32)
#define STR_LEN (32)

#define DIST_H(_a) ((pointnode_h)_a)
#define P3D_H(_a) &((float3Dnode_h)_a)->_data

#define getregion(ctx, scan) (ctx->_regions[scan._xyz._x][scan._xyz._y][scan._xyz._z])
#define scanregionstart(scan)                 \
    {                                         \
        uint3D_t scan = ctx->_regions_origin; \
        do
#define scanregionend(scan)                                                \
    while (!bscanuint3d(&scan, &ctx->_regions_origin, &ctx->_regions_end)) \
        ;                                                                  \
    }

#define RSIZE (10LU)
typedef struct context
{
    dll_head_t _inputpoints;
    dll_head_t _distances[10];
    dll_head_t _sortedpoints;
    dll_head_t _circuits;
    dll_head_t _parsers;

    dll_head_t _regions[RSIZE][RSIZE][RSIZE];
    uint3D_t _regions_origin;
    uint3D_t _regions_end;

    size_t _segments;

    float3Dnode_h _lowest;
    float3Dnode_h _highest;

    float _distancesegment;
    float _longestdistance;

    float _z_segment;
    size_t _circuitcount;

    size_t _result;
} context_t;
typedef context_t *context_h;

extern parser_t float3Dparser;

typedef struct pointnode
{
    dll_node_t _node;
    float3D_t *_f3d;
    dll_head_t _connections;
    struct pointnode *_closest;
    float _shortestdist;
    bool _scanned;
    size_t _maxdists;
    bool _connected;
} pointnode_t;

typedef pointnode_t *pointnode_h;
dll_node_h pointnode_ctor(float3D_h p3dref);

typedef_dllnode(pointref,
                pointnode_h _first_point;
                pointnode_h _second_point;
                float _dist;);

typedef pointrefnode_t *pointrefnode_h;
void pointrefnode_free(pointrefnode_h arg);

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c);
float p3ddist(float3D_h _a, float3D_h _b);
void dispatchpoints(context_h ctx);

void pointnode_free(void *arg);

void getallcircuitssizes1(context_h ctx, bool verbose);
void getallcircuitssizes2(context_h ctx, bool verbose);

void getcircuitsize(pointnode_h start, pointnode_h origin, bool verbose);
/**
 * 3D float coordinates point
 */
const char *const strfloat3d(float3D_h p3d_h, const char *const fmt);

/**
 * 3D point list
 */
void printpreflist(dll_head_h distances);
void printplist(dll_head_h points);

/**
 *  Distances from 2 3D points
 */

size_t scanregions(context_h ctx, bool verbose);
size_t missingmeasures(context_h ctx);

void sortdistances(dll_head_h head, dll_head_h dists);
void distancesnarrowscan(context_h ctx);
void distancesdoublescan(context_h ctx);
void distancessearchedscan(context_h ctx, bool verbose);

size_t peekcubicsubregion(context_h ctx, uint3D_t *center, bool verbose);

size_t getconnectionscount(pointnode_h startp, pointnode_h excl);
void getconnections(pointnode_h origin, pointnode_h current, pointnode_h from, dll_head_h pointlist, bool verbose);
void reinitconnections(pointnode_h origin, pointnode_h current, pointnode_h from, dll_head_h pointlist, bool verbose);

size_t removecircuit(pointnode_h startp, pointnode_h excl);
size_t connect_x_unconnectedpoints(context_h ctx, size_t x, bool verbose);
void connectalluntilonecircuit(context_h ctx, bool verbose);