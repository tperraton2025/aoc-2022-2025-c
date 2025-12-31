#ifndef AOC_PRIV_LL_DEFINES_H
#define AOC_PRIV_LL_DEFINES_H

#define typedef_dllnode(type, members) \
    typedef struct type                \
    {                                  \
        dll_node_t _node;              \
        members                        \
    } type##node_t;                    \
    typedef type##node_t *type##node_h;

#define typedef_dllhandle(type) \
    typedef type##_t *type##_h;

#define typenode_ctor(type, membersdef, ...)     \
    dll_node_h type##node_ctor(__VA_ARGS__)      \
    {                                            \
        type##node_t *new = NULL;                \
        new = calloc(1LU, sizeof(type##node_t)); \
        membersdef;                              \
        return &new->_node;                      \
    }

#define DLL_NODE_CTOR(_type, _name)        \
    _type *_name(void)                     \
    {                                      \
        _type *_ret = NULL;                \
        _ret = calloc(1LU, sizeof(_type)); \
        if (!_ret)                         \
            return NULL;                   \
        return _ret;                       \
    }

#define LL_FOREACH_P_EXT(_it, _head_h) \
    for (_it = _head_h->_first; _it != NULL; _it = _it->_next)

#define LL_FOREACH_P(_it, _head_h) \
    dll_node_h _it;                \
    for (_it = _head_h->_first; _it != NULL; _it = _it->_next)

#define LL_FOREACH_P_REV(_it, _head_h) \
    dll_node_h _it;                    \
    for (_it = _head_h->_last; _it != NULL; _it = _it->_prev)

#define LL_FOREACH(_it, _head) \
    dll_node_h _it;            \
    for (_it = _head._first; _it != NULL; _it = _it->_next)

#define LL_FOREACH_DBG(_it, _head) \
    dll_node_h _it;                \
    dll_node_h _itnext;            \
    for (_it = _head._first; _it != NULL; _it = _it->_next)

#define LL_FOREACH_EXT(_it, _head) \
    for (_it = _head._first; _it != NULL; _it = _it->_next)

#define LL_FOREACH_REV_EXT(_it, _head) \
    for (_it = _head._last; _it != NULL; _it = _it->_prev)

#define LL_FOREACH_REV(_it, _blk) \
    dll_node_h _it;               \
    LL_FOREACH_REV_EXT(_it, _blk)

#define LL_FOREACH_CODE_P(_it, _head, _code)                     \
    {                                                            \
        dll_node_h _it;                                          \
        for (_it = _head->_first; _it != NULL; _it = _it->_next) \
        {                                                        \
            _code                                                \
        }                                                        \
    }

#endif