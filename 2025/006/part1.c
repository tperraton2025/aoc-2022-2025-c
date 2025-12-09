#include "partx.h"

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{

    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);

    parser_append(&_ctx->parsers, &numparser, &_ctx->_prob);
    parser_append(&_ctx->parsers, &opsparser, &_ctx->_prob);

    _ctx->_result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->parsers, _blk->_str);

    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);

    size_t _len = _ctx->_prob._params[0]._size;

    dll_node_h _operands = _ctx->_prob._ops._first;

    checkparseconsistency(_ctx);

    LL_FOREACH(_param1, _ctx->_prob._params[0])
    {
        size_t _subresult = 0;
        if (((charlistitem_h)_operands)->_char == '+')
            _subresult = add(_ctx);

        else if (((charlistitem_h)_operands)->_char == '*')
            _subresult = multiply(_ctx);

        _ctx->_result += _subresult;

        ARR_FOREACH(_it, _ctx->_prob._params)
        {
            if (!_ctx->_prob._paramnodes[_it])
                break;
            _ctx->_prob._paramnodes[_it] = _ctx->_prob._paramnodes[_it]->_next;
        }
        _operands = _operands->_next;
        if (!_operands)
            break;
    }

    if (5361735137219 == _ctx->_result || 4277556 == _ctx->_result)
    {
        aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    }
    else
    {
        aoc_err("AOC %s %s solution is not %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    }
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);

    ARR_FOREACH(_it, _ctx->_prob._params)
    {
        dll_free_all(&_ctx->_prob._params[_it], intlistnode_free);
    }
    dll_free_all(&_ctx->_prob._ops, free);

    for (size_t _ii = 0; _ii < _ctx->_prob._instrcnt; _ii++)
        free(_ctx->_prob._instr[_ii]);

    free(_ctx->_prob._instr);

    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;