
#define SLEEP_TIME_MS (0)
#include <stdlib.h>
#include <string.h>

typedef struct command
{
    struct dll_node _node;
    size_t count;
    size_t from;
    size_t to;
} command_t;

static command_t *command_ctor(command_t *_ncmd)
{
    command_t *ret = malloc(sizeof(struct command));
    if (!ret)
        return NULL;
    memset(ret, 0, sizeof(struct command));
    ret->count = _ncmd->count;
    ret->from = _ncmd->from;
    ret->to = _ncmd->to;
    return ret;
}

typedef struct
{
    struct dll_node _node;
    aoc_2d_object_h data;
    int _blocked;
} aoc_2d_object_hracker_t;

static aoc_2d_object_hracker_t *aoc_2d_object_hracker(aoc_2d_object_h _obj)
{
    aoc_2d_object_hracker_t *_ntracker = malloc(sizeof(aoc_2d_object_hracker_t));
    if (!_ntracker)
        return NULL;
    _ntracker->_node._prev = NULL;
    _ntracker->_node._next = NULL;
    _ntracker->_blocked = false;
    _ntracker->data = _obj;
    return _ntracker;
}

static void aoc_2d_object_hracker_free(void *arg)
{
    aoc_2d_object_hracker_t *_ntracker = CAST(aoc_2d_object_hracker_t *, arg);
    FREE_AND_CLEAR_P(_ntracker);
}

typedef struct coord_tracker
{
    struct dll_node _node;
    coord_t _coord;
} coord_tracker_t;

DLL_NODE_CTOR(coord_tracker_t, coord_tracker);

static bool has_same_column(void *arga, void *argb)
{
    coord_t *_pos = (coord_t *)argb;
    return _pos->_x == (size_t)arga;
}

static struct dll_node *highest_column(struct dll_node *arga, struct dll_node *argb)
{
    coord_t *_posa = (coord_t *)arga;
    coord_t *_posb = (coord_t *)argb;
    return (struct dll_node *)(_posa->_x > _posb->_x ? _posa : _posb);
}

