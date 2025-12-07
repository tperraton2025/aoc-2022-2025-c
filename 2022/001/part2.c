#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct elfCal_t
{
    struct dll_node _node;
    char *_name;
    int _count;
    int _calories;
};

struct context
{
    struct dll_head _ll;
    size_t _result;
};

#define ELF_CAST(_p) ((struct elfCal_t *)_p)
#define CTX_CAST(_p) ((struct context *)_p->_data)

static struct elfCal_t *elf_ctor(struct context *_ctx)
{
    struct elfCal_t *_ret = malloc(sizeof(struct elfCal_t));
    if (!_ret)
        return NULL;

    _ret->_count = _ctx->_ll._size;
    _ret->_calories = 0;
    _ret->_name = malloc(sizeof("elfo999"));
    if (!_ret->_name)
        goto cleanup;
    sprintf(_ret->_name, "elfo%3d", _ret->_count);

    if (dll_node_append(&_ctx->_ll, (dll_node_h )_ret))
        goto name;
    return _ret;

name:
    FREE(_ret->_name);
cleanup:
    FREE(_ret);
    return NULL;
}

static void freeElf(void *_data)
{
    struct elfCal_t *_elf = ELF_CAST(_data);
    assert(_elf);
    assert(_elf->_name);
    FREE(_elf->_name);
    FREE(_elf);
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    int ret = 0;
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM; 

    struct context *_ctx = CAST(struct context *, _blk->_data);
    dll_head_init(&_ctx->_ll);

    if (elf_ctor(_ctx))
        return 0;
    return 1;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);

    int _cal = 0;
    if (sscanf(_blk->_str, "%d\n", &_cal))
        if (_cal)
        {
            ELF_CAST(_ctx->_ll._current)->_calories += _cal;
            LL_FOREACH(_node, _ctx->_ll)
            {
                if (ELF_CAST(_node)->_calories < ELF_CAST(_ctx->_ll._current)->_calories)
                {
                    dll_node_disconnect(&_ctx->_ll, _ctx->_ll._current);
                    dll_node_insert(&_ctx->_ll, _ctx->_ll._current, _node);
                    break;
                }
            }
        }
        else
        {
            if (elf_ctor(_ctx))
                return 0;
            else
                return 1;
        }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    int  count = 0;
    struct context *_ctx = CAST(struct context *, _blk->_data);

    LL_FOREACH(_node, _ctx->_ll)
    {
        if (count++ < 3)
            _ctx->_result += ELF_CAST(_node)->_calories;
    }
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    return 0;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    struct data_t *_data = CAST(struct data_t *, _blk->_data);
    dll_free_all(&_ctx->_ll, freeElf);
    FREE(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part2 = &privPart2;