
#define SLEEP_TIME_MS (0)
#include <aoc_parser.h>
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

static int parseblock(void *arg, char *_str);
static int parsecommand(void *arg, char *_str);

static struct parser blockparser = {._name = "block", ._func = parseblock};
static struct parser commandparser = {._name = "command", ._func = parsecommand};
