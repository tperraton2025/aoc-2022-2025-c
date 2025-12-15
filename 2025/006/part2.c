#include "partx.h"

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{

    TRY_TYPE_MALLOC(_blk->_data, struct context);
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);

    parser_append(&_ctx->parsers, &octonumparser, &_ctx->_prob);
    parser_append(&_ctx->parsers, &octoopsparser, &_ctx->_prob);

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

    checkocotparseconsistency(_ctx);
    char lastoperand = ' ';
    char *opstr = _ctx->_prob._opstr;
    size_t head = strlen(_ctx->_prob._instr[0]) - 1;

    base10_h conv = base_10_conversion(0);
    size_t rbuff[6] = {0};
    size_t ind = 0;

    while (head < SIZE_MAX)
    {

        base_10_clear(conv);

        for (size_t _decimal = 0; _decimal < _ctx->_prob._instrcnt; _decimal++)
        {
            conv->_digits[_ctx->_prob._instrcnt - 1 - _decimal] = _ctx->_prob._instr[_decimal][head] > '0' ? _ctx->_prob._instr[_decimal][head] - '0' : 0;
        }

        base_10_reverse_conversion(conv);

        if (conv->_val)
        {
            while (!(conv->_val % 10))
                conv->_val /= 10;

            rbuff[ind++] = conv->_val;
            if (ind >= ARRAY_DIM(rbuff))
                ind = 0;
        }
        else
        {
            memset(rbuff, 0, ARRAY_DIM(rbuff));
            ind = 0;
            head--;
            continue;
        }
        size_t delta = 0;

        if (opstr[head] == '*')
        {
            delta = rbuff[0] ? rbuff[0] : 1;
            for (size_t _ii = 1; _ii < ind; _ii++)
            {
                if (rbuff[_ii])
                    delta *= rbuff[_ii];
            }
            _ctx->_result += delta;
        }
        else if (opstr[head] == '+')
        {
            delta = rbuff[0];
            for (size_t _ii = 1; _ii < ind; _ii++)
            {
                delta += rbuff[_ii];
            }
            _ctx->_result += delta;
        }
        head--;
    }

    free(conv);
    if (55486328 < _ctx->_result || 3263827 == _ctx->_result)
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
        dll_free_all(&_ctx->_prob._params[_it], intb10item_free);
    }
    dll_free_all(&_ctx->_prob._ops, free);

    for (size_t _ii = 0; _ii < _ctx->_prob._instrcnt; _ii++)
        free(_ctx->_prob._instr[_ii]);

    free(_ctx->_prob._opstr);
    free(_ctx->_prob._instr);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;