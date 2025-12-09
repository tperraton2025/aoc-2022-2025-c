#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_numeric.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aoc_tree.h>
#include <unistd.h>
#include <aoc_helpers.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct joltage
{
    DLL_DECLARE()
    struct joltage *_other;
    size_t _rating;
    size_t _order;
} joltage_t;
typedef struct joltage *joltage_h;

joltage_h joltage_ctor();

bool has_same_rating(void *arg, void *prop);
bool came_after(void *arg, void *prop);
bool did_not_come_after(void *arg, void *prop);

dll_node_h pick_by_low_order(dll_node_h arga, dll_node_h argb);
dll_node_h comparebyrating(dll_node_h arga, dll_node_h argb);

typedef struct joltchoice
{
    tree_node_t _treenode;
    dll_head_h _originbyorder;
    dll_head_h _availablebyrating;
    joltage_h _selected;
    size_t _decimalscurrent;
    size_t _decimalsneeded;
    base10_h _conversion;
} joltchoice_t;

typedef joltchoice_t *joltchoice_h;

typedef struct context
{
    size_t _originlen;
    struct dll_head _originbyrating;
    struct dll_head _originbyorder;

    joltchoice_h _root;
    size_t _result;

} context_t;

typedef context_t *context_h;

context_h newcombination(context_h parent);
void populatejoltages(context_h _ctx, char *_str);

joltage_h picknextelligiblebyrating(context_h _ctx);
joltage_h picknextelligiblebyorder(context_h _ctx);
joltage_h pickearliestwithhighestbyrating(joltage_h _start);

static char _strcomb[2048] = {0};
static char *strcomb(joltchoice_h choice)
{
    struct dll_head path = {0};

    joltchoice_h firstchx = choice;
    while (firstchx)
    {
        void_dll_node_h step = malloc(sizeof(void_dll_node_t));
        step->_data = (void *)firstchx->_selected;
        dll_node_append(&path, &step->_node);
        firstchx = (joltchoice_h)firstchx->_treenode._parent;
    }
    void_dll_node_h pathnode = (void_dll_node_h)path._last;
    joltage_h lastchx = (joltage_h)pathnode->_data;

    char *_pen = &_strcomb[0];

    LL_FOREACH_P(_node, choice->_originbyorder)
    {
        assert((_pen - _strcomb < sizeof(_strcomb)));
        joltage_h jolth = (joltage_h)_node;

        if (lastchx)
        {
            if (lastchx->_order == jolth->_order)
            {
                _pen += sprintf(_pen, GREEN "%lu" RESET, jolth->_rating);
                pathnode = (void_dll_node_h)pathnode->_node._prev;
                lastchx = pathnode ? (joltage_h)pathnode->_data : NULL;
            }
            else
                _pen += sprintf(_pen, RED "%lu" RESET, jolth->_rating);
        }
        else
            _pen += sprintf(_pen, RED "%lu" RESET, jolth->_rating);
    }
    assert((_pen - _strcomb < sizeof(_strcomb)));

    dll_free_all(&path, free);
    return _strcomb;
}

static context_h makenextjoltchoice(context_h parent, joltage_h from)
{

    return NULL;
}

void freejoltchoice(void *arg);
joltchoice_h joltagetreechoice(joltchoice_h parent, size_t decimals, dll_head_h byrating, dll_head_h byorder, joltage_h selected, void free(void *arg));
size_t joltagepickallfeasible(joltchoice_h parent);
