
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
    free(_ntracker);
}