#ifndef AOC_PRIV_LL_TYPES_H
#define AOC_PRIV_LL_TYPES_H
#include <stdbool.h>
#include <stddef.h>

struct dll_head
{
    struct dll_node *_first;
    struct dll_node *_current;
    struct dll_node *_last;
    void (*freeall)(void *_data);
    size_t _size;
};

typedef struct dll_head *dll_head_h;
typedef struct dll_head dll_head_t;

struct dll_node
{
    struct dll_node *_prev;
    struct dll_node *_next;
    bool _obsolete;
};

typedef struct dll_node dll_node_t;
typedef struct dll_node *dll_node_h;

typedef dll_node_h(dll_compare)(dll_node_h _a, dll_node_h _b);
typedef dll_node_h(dll_exp_compare)(dll_node_h _a, dll_node_h _b, dll_node_h _c);

#endif
