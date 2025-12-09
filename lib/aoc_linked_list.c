#include "aoc_helpers.h"
#include "aoc_linked_list.h"
#include <errno.h>
#include <assert.h>

void dll_head_init(dll_head_h head)
{
    head->_first = NULL;
    head->_current = NULL;
    head->_last = NULL;
    head->_size = 0;
}

void dll_free_all(dll_head_h head, void (*_caller)(void *_data))
{
    assert(head && "Linked list context is NULL");
    dll_node_h _current = head->_first;
    if (!_current)
        goto end;
    dll_node_h _next = _current->_next;
    while (_current)
    {
        _next = _current->_next;
        if (_caller)
            _caller(_current);
        _current = _next;
        head->_size--;
    }
end:
    head->_current = NULL;
    head->_first = NULL;
    head->_last = NULL;
    assert(head->_size == 0 && "the link list was probably broken");
}

size_t dll_size(dll_head_h head)
{
    assert(head);
    return head->_size;
}

int dll_node_append(dll_head_h head, dll_node_h _new)
{
    assert(head && _new && "NULL pointer in dll_node_append");
    assert(head->_size < LL_MAX_LEN_LUI);

    head->_size++;
    if (head->_last)
    {
        _new->_prev = head->_last;
        head->_last->_next = _new;
        head->_last = _new;
    }
    else
    {
        head->_first = _new;
        head->_last = _new;
        _new->_prev = NULL;
    }
    head->_current = _new;
    _new->_next = NULL;
    return 0;
}

int dll_node_sorted_insert(dll_head_h head, dll_node_h _new, dll_compare sort)
{
    assert(head && _new && "NULL pointer in dll_node_append");
    if (head->_size > LL_MAX_LEN_LUI)
        return ENOSPC;
    if (!sort)
        return dll_node_append(head, _new);
    dll_node_h _afterNew = NULL;
    LL_FOREACH_P_EXT(_afterNew, head)
    {
        if (_new == sort(_afterNew, _new))
        {
            break;
        }
    }
    if (_afterNew)
        return dll_node_insert(head, _new, _afterNew);
    else
        return dll_node_append(head, _new);
}

void dll_node_swap(dll_head_h head, dll_node_h _a, dll_node_h _b)
{
    dll_node_h _anext = _a->_next;
    dll_node_h _aprev = _a->_prev;

    if (head->_first == _a || head->_first == _b)
        head->_first = head->_first == _a ? _b : _a;

    if (head->_last == _a || head->_last == _b)
        head->_last = head->_last == _a ? _b : _a;

    _a->_prev = _b->_prev == _a ? _b : _b->_prev;
    _a->_next = _b->_next == _a ? _b : _b->_next;

    if (_a->_prev)
        _a->_prev->_next = _a;
    if (_a->_next)
        _a->_next->_prev = _a;

    _b->_prev = _aprev == _b ? _a : _aprev;
    _b->_next = _anext == _b ? _a : _anext;

    if (_b->_prev)
        _b->_prev->_next = _b;
    if (_b->_next)
        _b->_next->_prev = _b;

    assert(_a->_prev != _a);
    assert(_a->_next != _a);

    assert(_b->_prev != _b);
    assert(_b->_next != _b);

    assert(head->_first != head->_last);
}

int dll_node_insert(dll_head_h head, dll_node_h _a, dll_node_h _b)
{
    assert(head && "Linked list head NULL");
    assert(_a && "Linked list _a is NULL");
    assert(_b && "Linked list _b is NULL");
    head->_size++;
    if (!_b->_prev)
        head->_first = _a;

    _a->_prev = _b->_prev;
    _a->_next = _b;
    if (_b->_prev)
        _b->_prev->_next = _a;
    _b->_prev = _a;
    return 0;
}

/*  Takes out node from the list without freeing its*/
void dll_node_disconnect(dll_head_h head, dll_node_h _a)
{
    assert(head->_size != 0 && "attempt to remove link from an empty sinked list");
    assert(_a && "NULL pointer provided");

    head->_size--;
    if (_a->_prev)
        _a->_prev->_next = _a->_next;
    else
        head->_first = _a->_next;
    if (_a->_next)
        _a->_next->_prev = _a->_prev;
    else
        head->_last = _a->_prev;
}

void dll_node_free(dll_head_h head, dll_node_h *_a, void func(void *))
{
    assert(head->_size != 0 && "attempt to remove link from an empty sinked list");
    assert(_a && "NULL pointer provided");
    dll_node_h _save = (*_a)->_next;
    dll_node_disconnect(head, *_a);
    free(*_a);
    *_a = _save;
}

int dll_find_node(dll_head_h head, dll_node_h _a)
{
    LL_FOREACH_P(_node, head)
    {
        if (!_node->_next)
            break;
        if (_node == _a)
            return 0;
    }
    return ENOENT;
}

size_t dll_count_nodes_by_property(dll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b))
{
    size_t ret = 0;
    bool _found = false;
    if (!head || !equal)
        return 0;
    LL_FOREACH_P(_node, head)
    {
        if (equal(_node, _prop))
            ret++;
    }
    return ret;
}

size_t dll_try_count_nodes_by_property(dll_node_h start, void *arg, bool (*equal)(void *_a, void *_b), bool (*stop)(void *_a, void *_b))
{
    if (!start || !equal || !stop)
        return 0;
    size_t result = 0;
    dll_node_h current = start->_next;
    while (current)
    {
        if (equal(current, arg))
            result++;
        if (stop(current, arg))
            break;
        current = current->_next;
    }
    current = start->_prev;
    while (current)
    {
        if (equal(current, arg))
            result++;
        if (stop(current, arg))
            break;
        current = current->_prev;
    }
    return result;
}

int dll_foreach_node_with_args(dll_head_h head, void *args, bool (*func)(void *_a, void *_b))
{
    if (!head || !func)
        return EINVAL;
    LL_FOREACH_P(_node, head)
    {
        func(_node, args);
    }
    return 0;
}

dll_node_h dll_find_node_by_property(dll_head_h head, void *_prop, bool (*equal)(void *_a, void *_b))
{
    if (!head || !equal)
        return NULL;
    LL_FOREACH_P(_node, head)
    {
        if (equal(_node, _prop))
            return _node;
    }
    return NULL;
}

dll_node_h dll_try_find_node_by_property(dll_node_h start, void *arg, bool (*equal)(void *_a, void *_b), bool (*stop)(void *_a, void *_b))
{
    if (!start || !equal || !stop)
        return NULL;
    dll_node_h current = start;
    while (current)
    {
        if (equal(current, arg))
            return current;
        if (stop(current, arg))
            break;
        current = current->_next;
    }
    current = start;
    while (current)
    {
        if (equal(current, arg))
            return current;
        if (stop(current, arg))
            break;
        current = current->_prev;
    }
    return NULL;
}

dll_head_h dll_clone(dll_head_h toclone, size_t nodesize)
{
    dll_head_h _clone = malloc(sizeof(dll_head_t));
    dll_head_init(_clone);

    LL_FOREACH_P(_node, toclone)
    {
        dll_node_h _copy = malloc(nodesize);
        memcpy(_copy, _node, nodesize);
        _copy->_next = NULL;
        _copy->_prev = NULL;
        dll_node_append(_clone, _copy);
    }
    return _clone;
}

/* TODO must find a solution without copy */
dll_head_h dll_clone_sorted(dll_head_h toclone, dll_compare(*comp), size_t nodesize)
{
    dll_head_h _sorted = malloc(sizeof(dll_head_t));
    dll_head_init(_sorted);

    LL_FOREACH_P(_node, toclone)
    {
        dll_node_h _nodecopy = malloc(nodesize);
        memcpy(_nodecopy, _node, nodesize);
        _nodecopy->_next = NULL;
        _nodecopy->_prev = NULL;
        dll_node_sorted_insert(_sorted, _nodecopy, comp);
    }
    return _sorted;
}
typedef struct
{
    struct dll_node _node;
    char *_str;
} teststr_t;

/* brutal while waiting for better */
int dll_sort(dll_head_h head, dll_compare(*comp))
{
    bool _sorted = false;
    while (!_sorted)
    {
        LL_FOREACH_P(_node, head)
        {
            if (_node->_next)
            {
                if (_node == comp(_node, _node->_next))
                {
                    dll_node_swap(head, _node, _node->_next);
                    break;
                }
            }
            else
                _sorted = true;
        }
    }
    return 0;
}

dll_head_h dll_clone_trimmed(dll_head_h head, size_t nodesize, void *_prop, bool (*remiftrue)(void *arg, void *prop))
{
    if (!head || !remiftrue)
        return NULL;

    dll_head_h _trimhead = malloc(sizeof(dll_head_t));
    dll_head_init(_trimhead);

    LL_FOREACH_P(_node, head)
    {
        if (remiftrue((void *)_node, _prop))
            continue;
        dll_node_h _nnode = malloc(nodesize);
        memcpy(_nnode, _node, nodesize);
        dll_node_append(_trimhead, _nnode);
    }
    return _trimhead;
}

int dll_trim_nodes(dll_head_h head, void *arg, bool (*remifeq)(void *_a, void *_b), void(free)(void *arg))
{
    if (!head || !remifeq || !free)
        return EINVAL;
    size_t liberated = 0;
    dll_node_t ghostnode = {0};
    LL_FOREACH_P(_node, head)
    {
        if (remifeq(_node, arg))
        {
            liberated++;
            ghostnode._next = _node->_next;
            dll_node_disconnect(head, _node);
            free(_node);
            if (!ghostnode._next)
                break;
            _node = &ghostnode;
        }
    }
    return liberated;
}

string_dll_node_h string_dll(const char *const name)
{
    string_dll_node_h _ret = malloc(sizeof(string_dll_node_t));
    if (!_ret)
        return NULL;
    _ret->_str = malloc(strnlen(name, MAX_STR_DLL_LEN) + 1);
    if (!_ret->_str)
        goto abort;

    strcpy(_ret->_str, name);
    return _ret;

abort:
    FREE(_ret);
    return NULL;
}

void string_dll_free(void *string)
{
    string_dll_node_h _stringh = (string_dll_node_h)string;
    FREE(_stringh->_str);
    FREE(_stringh);
}

dll_node_h string_dll_compare(dll_node_h _a, dll_node_h _b)
{
    string_dll_node_h _string_a = (string_dll_node_h)_a;
    string_dll_node_h _string_b = (string_dll_node_h)_b;
    int _ret = strcmp(_string_a->_str, _string_b->_str);
    if (_ret >= 0)
        return _b;
    else
        return _a;
}

int string_dll_rename(string_dll_node_h node, const char *const name)
{
    char *_nname = malloc(strnlen(name, MAX_STR_DLL_LEN) + 1);
    if (!_nname)
        return ENOMEM;
    FREE(node->_str);
    sprintf(_nname, "%s", name);
    node->_str = _nname;
    return 0;
}

void dll_foreach(dll_head_h head, void *arg, void(func)(void *arga, void *argb))
{
    LL_FOREACH_P(_node, head)
    {
        func((void *)_node, arg);
    }
}