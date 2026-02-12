#include "aoc_numeric.h"

base10_h base_10_conversion(size_t luint)
{
    base10_h _base10 = calloc(1LU, sizeof(base10_t));
    if (!_base10)
        return NULL;
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

size_t base_10_reverse_conversion(base10_h base10)
{
    size_t _ret = 0;
    size_t _pow = 1;
    for (size_t _ii = 0; _ii < ARRAY_DIM(base10->_digits); _ii++)
    {
        _ret += base10->_digits[_ii] * (_pow);
        _pow *= 10;
        if (base10->_digits[_ii] && _ii > base10->_lastdigit)
            base10->_lastdigit = _ii;
    }
    base10->_val = _ret;
    return _ret;
}

void base_10_clear(base10_h base10)
{
    memset(base10->_digits, 0, ARRAY_DIM(base10->_digits));
    base10->_val = 0;
}

static char _strconv[1024] = {0};

char *strconv(base10_h base10)
{

    base10_t _dummy = {0};
    _dummy._val = base10->_val;
    _dummy._lastdigit = base10->_lastdigit;

    for (size_t _ii = 0; _ii <= ARRAY_DIM(base10->_digits); _ii++)
    {
        _dummy._digits[_ii] = base10->_digits[_ii] + '0';
    }

    char *_at = _strconv;

    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "╔══════════════════════════╗\n");
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "║value: %*ld ║\n", 18, _dummy._val);
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "║  " GREEN "  %.16s" RESET "      ║\n", _dummy._digits);
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "║  " GREEN "  %.16s" RESET "      ║\n", &_dummy._digits[16]);
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "║  " GREEN "  %.16s" RESET "      ║\n", &_dummy._digits[32]);
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "║  " GREEN "  %.16s" RESET "      ║\n", &_dummy._digits[48]);
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "║  " GREEN "  %.16s" RESET "      ║\n", &_dummy._digits[48]);
    _at += snprintf(_at, sizeof(_strconv) - (_at - _strconv), "╚══════════════════════════╝\n");

    assert(_at < _strconv + sizeof(_strconv) && "_strconv overflow");
    return _strconv;
}

void print_base10(base10_h base10)
{
    base10_t _dummy = {0};

    for (size_t _ii = 0; _ii <= base10->_lastdigit; _ii++)
    {
        _dummy._digits[_ii] = base10->_digits[_ii] + '0';
    }
    aoc_info("%lu = %s", base10->_val, &_dummy._digits[0]);
}

void base10_endianness_flip(base10_h conv)
{
    size_t _len = ARRAY_DIM(conv->_digits);
    for (size_t _ii = 0; _ii < _len >> 1; _ii++)
    {
        conv->_digits[_ii] = conv->_digits[_len - 1 - _ii] ^ conv->_digits[_ii];
        conv->_digits[_len - 1 - _ii] = conv->_digits[_ii] ^ conv->_digits[_len - 1 - _ii];
        conv->_digits[_ii] = conv->_digits[_len - 1 - _ii] ^ conv->_digits[_ii];
    }
}

char *strrev(char *_str)
{
    size_t _len = strlen(_str);
    for (size_t _ii = 0; _ii < _len >> 1; _ii++)
    {
        _str[_ii] = _str[_len - 1 - _ii] ^ _str[_ii];
        _str[_len - 1 - _ii] = _str[_ii] ^ _str[_len - 1 - _ii];
        _str[_ii] = _str[_len - 1 - _ii] ^ _str[_ii];
    }
    _str[_len + 1] = '\0';
    return _str;
}

char *string_base10(base10_h base10)
{
    char *_str = malloc(base10->_lastdigit + 1);
    size_t _ii = 0;
    for (; _ii < base10->_lastdigit; _ii++)
    {
        _str[_ii] = base10->_digits[_ii] + '0';
    }
    _str[_ii + 1] = '\0';
    return _str;
}

int base_10_increment(base10_h base10)
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