#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_2d_dynarr.h>
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


typedef struct uint3D
{
    size_t _x;
    size_t _y;
    size_t _z;
} uint3D_t;
typedef uint3D_t *uint3D_h;
uint3D_t uint3_ctor(size_t x, size_t y, size_t z);


typedef struct float3D
{
    float _x;
    float _y;
    float _z;
} float3D_t;
typedef float3D_t *float3D_h;
float3D_t float3D_ctor(float x, float y, float z);

typedef struct distnode
{
    dll_node_t _node;
    float3D_t *_pointa;
    float3D_t *_pointb;
    float _dist;
} distnode_t;
typedef distnode_t *distnode_h;
distnode_h distnode_ctor();
#define DIST_H(_a) ((distnode_h)_a)

typedef struct float3Dn_h
{
    dll_node_t _node;
    float3D_t _data;
} float3Dn_h_t;
typedef float3Dn_h_t *float3Dn_h;
float3Dn_h float3Dn_ctor();
#define P3D_H(_a) &((float3Dn_h)_a)->_data

typedef struct context
{
    dll_head_t _rawjboxlist;
    dll_head_t _sortedjboxlist;
    dll_head_t _distances;
    dll_head_h ***_regions;
    size_t _segments;
    dll_head_t _parsers;

    float3D_t lowest;
    float3D_t highest;
    uint3D_t _regionssize;
    size_t _result;
} context_t;
typedef context_t *context_h;

extern parser_t float3Dparser;

dll_node_h is_B_or_C_nearest_A(dll_node_h _a, dll_node_h _b, dll_node_h _c);
float p3ddist(float3D_h _a, float3D_h _b);

const char *const strdist(float3D_h _a, float3D_h _b);
const char *const strp3D(float3D_h p3d_h);
const char *const printplist(dll_head_h head, const char *const fmt);

int sortdistances(dll_head_h head, dll_head_h dists);
void printdlist(dll_head_h head, const char *const fmt);