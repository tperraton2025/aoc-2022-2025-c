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
void stringnode_free(void *string);

typedef struct voidpnode
{
    struct dll_node _node;
    void *_ptr;
} voidpnode_t;
typedef voidpnode_t *voidpnode_h;
dll_node_h voidpnode_ctor(void *ptr);

typedef struct intnode
{
    struct dll_node _node;
    size_t _int;
} intnode_t;
typedef intnode_t *intnode_h;
dll_node_h intnode_ctor();

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
} intb10node_t;
typedef intb10node_t *intb10node_h;

intb10node_h intb10node_ctor();
void intb10node_free(void *arg);

typedef struct floatnode
{
    struct dll_node _node;
    float _val;
} floatnode_t;
typedef floatnode_t *floatnode_h;
dll_node_h floatnode_ctor(float val);

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
dll_node_h float3Dnode_ctor(const float3D_t const data);

dll_node_h intcomphighest(dll_node_h arga, dll_node_h argb);
#endif
