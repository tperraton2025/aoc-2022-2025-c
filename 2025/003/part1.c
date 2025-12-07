#include "partx.h"

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR, _blk->_name);

    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;

    struct context *_ctx = CTX_CAST(_blk->_data);

    _ctx->_tofind = 2;
    _ctx->_result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);

    dll_head_init(&_ctx->_originbyrating);
    dll_head_init(&_ctx->_originbyorder);

    populatejoltages(_ctx, _blk->_str);

    _ctx->_lastpickedind = 0;

    LL_FOREACH(_startjoltit, _ctx->_originbyrating)
    {
        _ctx->_pickedinlist[_ctx->_lastpickedind] = pickearliestwithhighestbyrating((joltage_h)_startjoltit);
        joltage_h _nextjoltage = picknextelligiblebyrating(_ctx);

        if (_nextjoltage)
        {
            _ctx->_pickedinlist[++_ctx->_lastpickedind] = (joltage_h)_nextjoltage;
            break;
        }
        else
        {
            _ctx->_lastpickedind = 0;
            ARR_FOREACH(_it, _ctx->_pickedinlist)
            {
                _ctx->_pickedinlist[_it] = 0;
            }
            continue;
        }
    }

    _ctx->_conversion = base_10_conversion(0);
    _ctx->_conversion->_lastdigit = _ctx->_tofind;

    for (size_t _it = 0; _it < _ctx->_tofind; _it++)
    {
        _ctx->_conversion->_digits[_ctx->_tofind - _it - 1] = _ctx->_pickedinlist[_it]->_rating;
    }
    base_10_reverse_conversion(_ctx->_conversion);

    aoc_info("selected %s +%lu", strcomb(_ctx), _ctx->_conversion->_val);

    _ctx->_result += _ctx->_conversion->_val;

    dll_free_all(&_ctx->_originbyrating, free);
    dll_free_all(&_ctx->_originbyorder, free);

    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    int result = _ctx->_result;
    aoc_ans("AOC %s %s solution is %i", CONFIG_YEAR, _blk->_name, result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;
