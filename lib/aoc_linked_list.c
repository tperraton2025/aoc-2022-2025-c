#include "aoc_helpers.h"
#include "aoc_linked_list.h"
#include <errno.h>
#include <assert.h>

void dll_head_init(aoc_ll_head_h _blk)
{
    _blk->_first = NULL;
    _blk->_current = NULL;
    _blk->_last = NULL;
    _blk->_size = 0;
}

void dll_free_all(aoc_ll_head_h _blk, void (*_caller)(void *_data))
{
    assert(_blk && "Linked list context is NULL");
    struct dll_node *_current = _blk->_first;
    if (!_current)
        goto end;
    struct dll_node *_next = _current->_next;
    while (_current)
    {
        _next = _current->_next;
        if (_caller)
            _caller(_current);
        _current = _next;
        _blk->_size--;
    }
end:
    _blk->_current = NULL;
    _blk->_first = NULL;
    _blk->_last = NULL;
    assert(_blk->_size == 0 && "the link list was probably broken");
}

size_t aoc_dll_size(aoc_ll_head_h _ctx)
{
    assert(_ctx);
    return _ctx->_size;
}

int dll_node_append(aoc_ll_head_h _blk, struct dll_node *_new)
{
    assert(_blk && _new && "NULL pointer in dll_node_append");
    assert(_blk->_size < LL_MAX_LEN);

    _blk->_size++;
    if (_blk->_last)
    {
        _new->_prev = _blk->_last;
        _blk->_last->_next = _new;
        _blk->_last = _new;
    }
    else
    {
        _blk->_first = _new;
        _blk->_last = _new;
        _new->_prev = NULL;
    }
    _blk->_current = _new;
    _new->_next = NULL;
    return 0;
}

void dll_node_permut(aoc_ll_head_h _ctx, struct dll_node *_a, struct dll_node *_b)
{
    struct dll_node *_anext = _a->_next;
    struct dll_node *_aprev = _a->_prev;

    if (_ctx->_first == _a || _ctx->_first == _b)
        _ctx->_first = _ctx->_first == _a ? _b : _a;

    if (_ctx->_last == _a || _ctx->_last == _b)
        _ctx->_last = _ctx->_last == _a ? _b : _a;

    _a->_prev = _b->_prev;
    _a->_next = _b->_next;

    if (_a->_prev)
        _a->_prev->_next = _a;
    if (_a->_next)
        _a->_next->_prev = _a;

    _b->_prev = _aprev;
    _b->_next = _anext;

    if (_b->_prev)
        _b->_prev->_next = _b;
    if (_b->_next)
        _b->_next->_prev = _b;
}

void dll_node_insert(aoc_ll_head_h _ctx, struct dll_node *_a, struct dll_node *_b)
{
    assert(_ctx && "Linked list _ctx NULL");
    assert(_a && "Linked list _a is NULL");
    assert(_b && "Linked list _b is NULL");
    _ctx->_size++;
    if (!_b->_prev)
        _ctx->_first = _a;

    _a->_prev = _b->_prev;
    _a->_next = _b;
    if (_b->_prev)
        _b->_prev->_next = _a;
    _b->_prev = _a;
}

/*  Takes out node from the list without freeing its*/
void dll_node_disconnect(aoc_ll_head_h _ctx, struct dll_node *_a)
{
    assert(_ctx->_size != 0 && "attempt to remove link from an empty sinked list");
    assert(_a && "NULL pointer provided");

    _ctx->_size--;
    if (_a->_prev)
        _a->_prev->_next = _a->_next;
    else
        _ctx->_first = _a->_next;
    if (_a->_next)
        _a->_next->_prev = _a->_prev;
    else
        _ctx->_last = _a->_prev;
    _a->_prev = NULL;
    _a->_next = NULL;
}

int dll_find_node(aoc_ll_head_h _blk, struct dll_node *_a)
{
    LL_FOREACH_P(_node, _blk)
    {
        if (!_node->_next)
            break;
        if (_node == _a)
            return 0;
    }
    return ENOENT;
}

size_t dll_count_nodes_by_property(aoc_ll_head_h _blk, void *_prop, bool (*equal)(void *_a, void *_b))
{
    size_t ret = 0;
    bool _found = false;
    if (!_blk || !equal)
        return 0;
    LL_FOREACH_P(_node, _blk)
    {
        _found = equal(_node, _prop);
        if (true == _found)
            ret++;
    }
    return ret;
}

struct dll_node *dll_find_node_by_property(aoc_ll_head_h _blk, void *_prop, bool (*equal)(void *_a, void *_b))
{
    bool _found = false;
    if (!_blk || !equal)
        return NULL;
    LL_FOREACH_P(_node, _blk)
    {
        _found = equal(_node, _prop);
        if (true == _found)
            return _node;
    }
    return NULL;
}

// TODO: finish this
void dll_sort(aoc_ll_head_h _blk, ll_compare(*comp))
{
    LL_FOREACH_P(_node, _blk)
    {
        if (!_node->_next)
            break;
        if (_node == comp(_node, _node->_next))
            dll_node_permut(_blk, _node, _node->_next);
    }
}