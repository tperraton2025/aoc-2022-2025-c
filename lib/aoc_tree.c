#include "aoc_helpers.h"
#include "aoc_tree.h"
#include <errno.h>
#include <assert.h>

struct tree_node_t
{
    struct tree_node_t *_parent;
    struct ll_context_t _children;
    void (*free)(void *_data);
};

struct tree_blk_t
{
    struct tree_node_t *_root;
    size_t _size;
    void *_data;
};

struct tree_node_t *aoc_tree_node(struct tree_node_t *parent)
{
    struct tree_node_t *ret = malloc(sizeof(struct tree_node_t));
    ret->_parent = parent;
    ll_blk_init(&ret->_children);
    return ret;
}

void tree_blk_init(struct tree_blk_t *_blk)
{
    _blk->_root = aoc_tree_node(NULL);
    _blk->_size = 1;
    _blk->_root->_parent = NULL;
}

int tree_node_append(struct tree_node_t *_parent, struct tree_node_t *_new)
{
    ll_node_append(&_parent->_children, NODE_CAST(_new));
    _new->_parent = _parent;
}

void tree_free_all(struct tree_blk_t *_blk, void (*_caller)(void *_data))
{
    struct tree_node_t *ret = aoc_tree_leaf_node(_blk->_root);
    ll_free_all(&ret->_children, ret->free);
}

void aoc_tree_permut(struct tree_blk_t *_ctx, struct tree_node_t *_a, struct tree_node_t *_b)
{
}

void aoc_tree_insert(struct tree_blk_t *_ctx, struct tree_node_t *_a, struct tree_node_t *_b)
{
}

void aoc_tree_remove(struct tree_blk_t *_ctx, struct tree_node_t *_a)
{
}

size_t aoc_tree_size(struct tree_blk_t *_ctx)
{
}

struct tree_node_t *aoc_tree_leaf_node(struct tree_node_t *_parent)
{
    struct tree_node_t *ret = NULL;
    assert(_parent && "tree node is NULL");
    while (_parent->_children._first)
    {
        ret = _parent->_children._first;
    }
    return ret;
}