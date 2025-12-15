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

#define DIST_H(_a) ((distnode_h)_a)
#define P3D_H(_a) &((float3Dnode_h)_a)->_data

#define getregion(ctx, scan) ((dll_head_h *)dyn3dget(ctx->_regions, scan))

typedef struct context
{
    dll_head_t _rawjboxlist;
    dll_head_t _distances;
    dll_head_t _parsers;

    dyn3darr_h _regions;
    size_t _segments;

    float3Dnode_h lowest;
    float3Dnode_h highest;
    uint3D_t _regionssize;
    size_t _result;
} context_t;
typedef context_t *context_h;

extern parser_t float3Dparser;

typedef struct distnode
{
    dll_node_t _node;
    float3D_t *_pointa;
    float3D_t *_pointb;
    float _val;
} distnode_t;
typedef distnode_t *distnode_h;
distnode_h distnode_ctor();

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c);
float p3ddist(float3D_h _a, float3D_h _b);
void dispatchpoints(context_h ctx);
void freeregion(void *arg);

/**
 * 3D float coordinates point
 */
const char *const strfloat3d(float3D_h p3d_h);

/**
 * 3D point list
 */
void printplist(dll_head_h head, const char *const fmt);

/**
 *  Distances from 2 3D points
 */
const char *const strdist(distnode_h _dist, const char *const fmt);

size_t printregions(context_h ctx);
size_t missingmeasures(dll_head_h head);

void sortdistances(dll_head_h head, dll_head_h dists);
void measuredistances(context_h ctx);
size_t printdlist(dll_head_h head, const char *const fmt);