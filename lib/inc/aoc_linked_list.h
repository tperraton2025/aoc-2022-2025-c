#ifndef AOC_LL
#define AOC_LL

#include <stdbool.h>

#define LL_MAX_LEN (65535)
#define NODE_CAST(_p) ((struct dll_node *)_p)

struct dll_head
{
    struct dll_node *_first;
    struct dll_node *_current;
    struct dll_node *_last;
    size_t _size;
};

typedef struct dll_head *aoc_ll_head_h;

struct dll_node
{
    struct dll_node *_prev;
    struct dll_node *_next;
    void (*free)(void *_data);
};

typedef struct dll_node *(ll_compare)(struct dll_node * _a, struct dll_node *_b);

void dll_head_init(aoc_ll_head_h _blk);
void dll_sort(aoc_ll_head_h _blk, ll_compare(*comp));
int dll_node_append(aoc_ll_head_h _blk, struct dll_node *_new);
void dll_free_all(aoc_ll_head_h _blk, void (*_caller)(void *_data));
void dll_node_permut(aoc_ll_head_h _ctx, struct dll_node *_a, struct dll_node *_b);
void dll_node_insert(aoc_ll_head_h _ctx, struct dll_node *_a, struct dll_node *_b);
void dll_node_disconnect(aoc_ll_head_h _ctx, struct dll_node *_a);
size_t aoc_dll_size(aoc_ll_head_h _ctx);
int dll_find_node(aoc_ll_head_h _blk, struct dll_node *_a);
struct dll_node *dll_find_node_by_property(aoc_ll_head_h _blk, void *_prop, bool (*equal)(void *_a, void *_b));
size_t dll_count_nodes_by_property(aoc_ll_head_h _blk, void *_prop, bool (*equal)(void *_a, void *_b));

#define LL_DECLARE(_type) \
    struct _type *_prev;  \
    struct _type *_next;

#define LL_CTOR(_type, _name)                              \
    static struct _type *_name(struct _type *_prev)        \
    {                                                      \
        struct _type *_ret = malloc(sizeof(struct _type)); \
        _ret->_prev = _prev;                               \
        if (_prev)                                         \
            _prev->_next = _ret;                           \
        _ret->_next = NULL;                                \
        return _ret;                                       \
    }

#define LL_FOREACH_P(_it, _pblk)                             \
    struct dll_node *_it;                                   \
    for (_it = _pblk->_first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH(_it, _blk)                              \
    struct dll_node *_it;                                 \
    for (_it = _blk._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_EXT(_it, _blk)                          \
    for (_it = _blk._first; _it != NULL; _it = _it->_next) \
        if (_it)

#define LL_FOREACH_LAST_EXT(_it, _blk)                    \
    for (_it = _blk._last; _it != NULL; _it = _it->_prev) \
        if (_it)

#define LL_FOREACH_LAST(_it, _blk) \
    struct dll_node *_it;         \
    LL_FOREACH_LAST_EXT(_it, _blk)

#endif