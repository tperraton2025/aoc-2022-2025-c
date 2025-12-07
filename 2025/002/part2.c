#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_numeric.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_SSTR_LEN (1024)

struct context
{
    char *_instr;
    size_t _result;
};

#define CTX_CAST(_p) ((struct context *)_p) 
static int base_10_has_pattern_alt(base10_h base10);
static int base_10_has_pattern(base10_h base10, size_t _len);

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    aoc_info("Welcome to AOC %s %s", CONFIG_YEAR,  _blk->_name);
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    memset(_blk->_data, 0, sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    size_t _len = strnlen(_blk->_str, MAX_SSTR_LEN);
    _ctx->_instr = malloc(_len);
    memset(_ctx->_instr, 0, _len);
    strcpy(_ctx->_instr, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    if (!_ctx->_instr)
        return EINVAL;
    char *_sstr = strtok(_ctx->_instr, ",");
    size_t _first = 0;
    size_t _second = 0;
    size_t _ranges = 0;
    while (_sstr)
    {
        if (2 == sscanf(_sstr, "%lu-%lu", &_first, &_second))
        {
            _ranges++;
            assert(_first < _second);
            base10_h _conv = base_10_conversion(_first);
            while (_conv->_val <= _second)
            {
                size_t top = 0;
                size_t bot = 0;
                size_t _digits = 1;
                for (; _digits <= _conv->_lastdigit >> 1; _digits++)
                {
                    if (base_10_has_pattern(_conv, _digits))
                    {
                        _ctx->_result += _conv->_val;
                        break;
                    }
                }
                base_10_increment(_conv);
            }
            free(_conv);
        }
        _sstr = strtok(NULL, ",");
    }
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    assert(4174379265 == _ctx->_result || 50857215650 == _ctx->_result);
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

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;
 
static int base_10_has_pattern(base10_h base10, size_t _len)
{
    size_t _check = base10->_lastdigit / _len;

    if (_check * _len == base10->_lastdigit || _len == 1)
    {
        base10_t _dummy = {0};
        memcpy(&_dummy, base10, sizeof(base10_t));
        for (size_t _ii = 0; _ii <= base10->_lastdigit; _ii++)
        {
            _dummy._digits[_ii] = base10->_digits[_ii] + '0';
        }
        char *top = malloc((_len) + 1);
        char *bot = malloc((_len) + 1);
        size_t _repcntr = 1;
        for (size_t _sublen = 0; _sublen + _len < base10->_lastdigit; _sublen += _len)
        {
            memset(top, 0, _len + 1);
            memset(bot, 0, _len + 1);
            strncpy(top, _dummy._digits + _sublen, _len);
            strncpy(bot, _dummy._digits + _sublen + _len, _len);
            if (strncmp(top, bot, _len) || bot[_len - 1] == '0' || top[_len - 1] == '0')
            {
                FREE(bot);
                FREE(top);
                return 0;
            }
            _repcntr++;
        }
        FREE(bot);
        FREE(top);
        return 1;
    }
    return 0;
}

static int base_10_has_pattern_alt(base10_h base10)
{
    if (base10->_lastdigit % 2 == 0)
    {
        size_t pow = 1;
        for (size_t _it = 0; _it < base10->_lastdigit >> 1; _it++)
        {
            pow *= 10;
        }

        size_t top = base10->_val / (pow);
        size_t bot = base10->_val - (top * (pow));

        if (bot == top)
        {
            aoc_info("%20lu repeats %lu|%lu", base10->_val, top, bot);
            return 1;
        }
    }
    return 0;
}
 