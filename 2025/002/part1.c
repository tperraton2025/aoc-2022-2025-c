#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_SSTR_LEN (1024)

typedef struct base10
{
    char _digits[20];
    size_t _lastdigit;
    size_t _val;
} base10_t;
typedef struct base10 *base10_h;

struct context
{
    char *_instr;
    long unsigned result;
};

#define CTX_CAST(_p) ((struct context *)_p)
static void print_base10(base10_h base10);
static base10_h base_10_conversion(size_t luint);
static int base_10_has_pattern_alt(base10_h base10);
static int base_10_has_pattern(base10_h base10);
static int base_10_increment(base10_h base10);
static size_t base_10_reverse_conversion(base10_h base10);

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
                if (base_10_has_pattern(_conv))
                {
                    _ctx->result += _conv->_val;
                }
                base_10_increment(_conv);
            }
            free(_conv);
        }
        _sstr = strtok(NULL, ",");
    }
    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->result);
    assert(1227775554 == _ctx->result || 40055209690 == _ctx->result);
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

static base10_h base_10_conversion(size_t luint)
{
    base10_h _base10 = malloc(sizeof(base10_t));
    if (!_base10)
        return NULL;
    memset(_base10, 0, sizeof(base10_t));
    _base10->_val = luint;
    for (size_t _ii = 0; _ii < ARRAY_DIM(_base10->_digits) && luint; _ii++)
    {
        _base10->_lastdigit++;
        _base10->_digits[_ii] = (char)(luint % 10);
        luint = luint / 10;
    }
    size_t rconv = base_10_reverse_conversion(_base10);
    if (_base10->_val != rconv)
    {
        aoc_err("initial conv failed with %ld != %ld", _base10->_val, rconv);
    }
    return _base10;
}

static size_t base_10_reverse_conversion(base10_h base10)
{
    size_t _ret = 0;
    size_t _pow = 1;
    for (size_t _ii = 0; _ii < base10->_lastdigit; _ii++)
    {
        _ret += base10->_digits[_ii] * (_pow);
        _pow *= 10;
    }
    return _ret;
}

static void print_base10(base10_h base10)
{
    base10_t _dummy = {0};

    for (size_t _ii = 0; _ii <= base10->_lastdigit; _ii++)
    {
        _dummy._digits[_ii] = base10->_digits[_ii] + '0';
    }
    aoc_info("%lu = %s", base10->_val, &_dummy._digits[0]);
}

static int base_10_has_pattern(base10_h base10)
{
    int _ret = 0;
    base10_t _dummy = {0};
    memcpy(&_dummy, base10, sizeof(base10_t));
    for (size_t _ii = 0; _ii <= base10->_lastdigit; _ii++)
    {
        _dummy._digits[_ii] = base10->_digits[_ii] + '0';
    }
    if (base10->_lastdigit % 2 == 0)
    {
        size_t _len = _dummy._lastdigit >> 1;
        char *top = malloc((_len) + 1);
        char *bot = malloc((_len) + 1);
        memset(top, 0, _len + 1);
        memset(bot, 0, _len + 1);
        strncpy(top, _dummy._digits, _len);
        strncpy(bot, _dummy._digits + _len, _len);
        if (0 == strncmp(top, bot, _len))
            _ret = 1;
        FREE_AND_CLEAR_P(bot);
        FREE_AND_CLEAR_P(top);
        return _ret;
    }
    else
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
            return 1;
        }
    }
    return 0;
}

static int base_10_increment(base10_h base10)
{
    base10->_val++;
    for (size_t _ii = 0; _ii < ARRAY_DIM(base10->_digits); _ii++)
    {
        if (++base10->_digits[_ii] >= 10)
        {
            if (_ii >= base10->_lastdigit - 1)
                base10->_lastdigit++;

            base10->_digits[_ii] %= 10;
            if (_ii == ARRAY_DIM(base10->_digits) - 1)
                return EOVERFLOW;
        }
        else
            break;
    }

    size_t rconv = base_10_reverse_conversion(base10);
    if (base10->_val != rconv)
    {
        aoc_err("increment failed with %ld != %ld", base10->_val, rconv);
    }
    return 0;
}
