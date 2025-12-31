#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    int _encoder;
    size_t _result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    _blk->_data = calloc(1LU, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    struct context *_ctx = CTX_CAST(_blk->_data);
    _ctx->_encoder = 50;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    int _increments = 0;
    int _prevEncoder = _ctx->_encoder;
    int _newEncoder = _ctx->_encoder;

    if (1 == sscanf(_blk->_str, "R%i", &_increments))
        _newEncoder += _increments;
    else if (1 == sscanf(_blk->_str, "L%i", &_increments))
        _newEncoder -= _increments;
    if (0 > _newEncoder)
        _ctx->_encoder = ((int)100) + _newEncoder;
    else
        _ctx->_encoder = _newEncoder;
    _ctx->_encoder = _ctx->_encoder % 100;
    if (!_ctx->_encoder)
        _ctx->_result++;
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    /* free allocated _blk->_data members first*/
    /* solution specific section */
    /* free _blk->_data itself after wards */
    if (_blk->_data)
        free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part1 = &privPart1;