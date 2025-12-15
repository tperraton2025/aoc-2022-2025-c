#ifndef AOC_LL_GEN_NODES_H
#define AOC_LL_GEN_NODES_H
#include "aoc_numeric.h"
#include "ll_types.h"
#include <stdbool.h>

#define MAX_STR_DLL_LEN (1024)
/* c standard types linked list node constructors */

typedef struct stringnode
{
    struct dll_node _node;
    char *_str;
} stringnode_t;
typedef stringnode_t *stringnode_h;

typedef struct voidnode
{
    struct dll_node _node;
    void *_data;
} voidnode_t;
typedef voidnode_t *voidnode_h;

typedef struct intlistitem
{
    struct dll_node _node;
    size_t _int;
} intlistitem_t;
typedef intlistitem_t *intlistitem_h;
intlistitem_h intlistnode_ctor();

typedef struct charlistitem
{
    struct dll_node _node;
    char _char;
} charlistitem_t;
typedef charlistitem_t *charlistitem_h;

charlistitem_h charlistitem_ctor();

typedef struct intb10item
{
    struct dll_node _node;
    base10_h _int;
} intb10item_t;
typedef intb10item_t *intb10item_h;

intb10item_h intb10item_ctor();
void intb10item_free(void *arg);

typedef struct floatnode
{
    struct dll_node _node;
    float _val;
} floatnode_t;
typedef floatnode_t *floatnode_h;
floatnode_h floatnode_ctor(float val);

typedef struct float3D
{
    float _x;
    float _y;
    float _z;
} float3D_t;
typedef float3D_t *float3D_h;
float3D_t float3D_ctor(float x, float y, float z);

typedef struct float3Dnode_h
{
    dll_node_t _node;
    float3D_t _data;
} float3Dnode_t;
typedef float3Dnode_t *float3Dnode_h;
float3Dnode_h float3Dnode_ctor(const float3D_t const data);

#endif
