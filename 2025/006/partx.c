#include "partx.h"

int parsenum(void *arg, char *_str)
{
    bool _anynummatch = false;
    problem_t *_prob = (problem_t *)arg;

    size_t _strlen = strlen(_str);
    if (_strlen > 3686)
    {
        aoc_err("_len = %lu", _strlen)
    }

    char *_strbuf = malloc(_strlen + 10);
    snprintf(_strbuf, _strlen + 10, "%s", _str);
    char *_strpos = strtok(_strbuf, " ");

    size_t _newnum = 0;
    size_t _matches = 0;

    if (strchr(_str, '*') || strchr(_str, '+'))
        goto abort;

    while (_strpos - _strbuf < _strlen)
    {

        if (1 == sscanf(_strpos, "%lu", &_newnum))
        {
            intb10node_h new = intb10node_ctor();
            new->_int = base_10_conversion(_newnum);
            dll_node_append(&_prob->_params[_prob->_line], &new->_node);
            _anynummatch = true;
            _matches++;
        }
        _strpos = strtok(NULL, " ");
        if (_strpos == _strbuf)
            goto abort;
    }
    if (_anynummatch)
    {
        _prob->_line++;
        if (_prob->_line < ARRAY_DIM(_prob->_params))
            goto abort;
    }
    free(_strbuf);
    return _matches;
abort:
    free(_strbuf);
    return EINVAL;
}

int parseops(void *arg, char *_str)
{
    bool _anymatch = false;
    problem_t *_prob = (problem_t *)arg;

    size_t _strlen = strlen(_str);

    if (_strlen > 3686)
    {
        aoc_err("_len = %lu", _strlen)
    }

    char *_strbuf = malloc(_strlen + 1);
    snprintf(_strbuf, _strlen + 1, "%s", _str);
    char *_strpos = strtok(_strbuf, " ");

    char _new_op = '+';
    size_t _matches = 0;

    while ((_strpos - _strbuf) < _strlen)
    {
        if (('+' != *_strpos) && ('-' != *_strpos) && ('*' != *_strpos))
            goto abort;

        if (1 == sscanf(_strpos, "%c", &_new_op))
        {
            charlistitem_h new = charlistitem_ctor();
            new->_char = _new_op;

            dll_node_append(&_prob->_ops, &new->_node);
            _anymatch = true;
            _matches++;
        }
        _strpos = strtok(NULL, " ");
        if (_strpos == _strbuf)
            goto abort;
    }

    free(_strbuf);
    return _matches;

abort:
    free(_strbuf);
    return EINVAL;
}

parser_t numparser = {._func = parsenum, ._name = "nums"};
parser_t opsparser = {._func = parseops, ._name = "ops"};

DLL_NODE_CTOR(charlistitem_t, charlistitem_ctor);

void checkparseconsistency(context_h _ctx)
{
    size_t _len = _ctx->_prob._params[0]._size;
    ARR_FOREACH(_it, _ctx->_prob._params)
    {
        if (_it >= _ctx->_prob._line)
            break;
        _ctx->_prob._paramnodes[_it] = _ctx->_prob._params[_it]._first;
        if (_ctx->_prob._params[_it]._size != _len)
            aoc_err("%s", "parsing error");
    }
}

void checkoctoparseconsistency(context_h _ctx)
{
    size_t _len = strlen(_ctx->_prob._instr[0]);
    for (size_t _it = 0; _it < _ctx->_prob._instrcnt; _it++)
    {
        if (strlen(_ctx->_prob._instr[_it]) != _len)
            aoc_err("%s", "parsing error");
    }
}

size_t add(context_h _ctx)
{
    size_t _subresult = 0;
    ARR_FOREACH(_it, _ctx->_prob._params)
    {
        if (!_ctx->_prob._paramnodes[_it])
            break;
        _subresult += ((intb10node_h)_ctx->_prob._paramnodes[_it])->_int->_val;
    }
    return _subresult;
}

size_t multiply(context_h _ctx)
{
    size_t _subresult = 1;
    ARR_FOREACH(_it, _ctx->_prob._params)
    {
        if (!_ctx->_prob._paramnodes[_it])
            break;
        _subresult *= ((intb10node_h)_ctx->_prob._paramnodes[_it])->_int->_val;
    }
    return _subresult;
}

size_t octoadd(context_h _ctx)
{
    size_t _subresult = 0;
    size_t _highestorder = 0;
    base10_h _octoconv = base_10_conversion(0);

    ARR_FOREACH(_it, _ctx->_prob._params)
    {
        if (!_ctx->_prob._paramnodes[_it])
            break;
        _highestorder = HIGHEST(_highestorder, ((intb10node_h)_ctx->_prob._paramnodes[_it])->_int->_lastdigit);
    }
    for (size_t _order = _highestorder; _order < SIZE_MAX; _order--)
    {
        ARR_FOREACH_REV(_it, _ctx->_prob._params)
        {
            if (!_ctx->_prob._paramnodes[_it])
                continue;
            _octoconv->_digits[_it] = ((intb10node_h)_ctx->_prob._paramnodes[_it])->_int->_digits[_order];
        }
        _subresult += base_10_reverse_conversion(_octoconv);
        aoc_ans("+%lu", _octoconv->_val);
        aoc_info("%s", strconv(_octoconv));
    }

    free(_octoconv);
    return _subresult;
}

size_t octomult(context_h _ctx)
{
    size_t _subresult = 1;
    size_t _highestorder = 0;
    base10_h _octoconv = base_10_conversion(0);

    ARR_FOREACH(_it, _ctx->_prob._params)
    {
        if (!_ctx->_prob._paramnodes[_it])
            break;
        _highestorder = HIGHEST(_highestorder, ((intb10node_h)_ctx->_prob._paramnodes[_it])->_int->_lastdigit);
    }
    for (size_t _order = _highestorder; _order < SIZE_MAX; _order--)
    {
        ARR_FOREACH_REV(_it, _ctx->_prob._params)
        {
            if (!_ctx->_prob._paramnodes[_it])
                break;
            _octoconv->_digits[_it] = ((intb10node_h)_ctx->_prob._paramnodes[_it])->_int->_digits[_order];
        }
        _subresult *= base_10_reverse_conversion(_octoconv);
        aoc_ans("x%lu", _octoconv->_val);
        aoc_ans("%s", strconv(_octoconv));
    }

    free(_octoconv);
    return _subresult;
}

int octoparsenum(void *arg, char *_str)
{
    problem_t *_prob = (problem_t *)arg;

    size_t _strlen = strlen(_str);
    if (_strlen > 3686)
    {
        aoc_err("_len = %lu", _strlen)
    }

    if (strchr(_str, '*') || strchr(_str, '+'))
        return EINVAL;

    if (!_prob->_instr)
        _prob->_instr = malloc(sizeof(char *));
    else
        _prob->_instr = realloc(_prob->_instr, sizeof(char *) * (_prob->_instrcnt + 1));

    _prob->_instr[_prob->_instrcnt] = malloc(_strlen + 10);
    snprintf(_prob->_instr[_prob->_instrcnt], _strlen + 10, "%s", _str);

    _prob->_instrcnt++;

    return 0;
}

int octoparseops(void *arg, char *_str)
{
    problem_t *_prob = (problem_t *)arg;

    size_t _strlen = strlen(_str);
    if (_strlen > 3686)
    {
        aoc_err("_len = %lu", _strlen)
    }

    if (!N_BETWEEN_AB(*_str, '*', '/'))
        return EINVAL;

    if (!_prob->_instr)
        _prob->_instr = malloc(sizeof(char *));
    else
        _prob->_instr = realloc(_prob->_instr, sizeof(char *) * (_prob->_instrcnt + 1));

    _prob->_opstr = malloc(_strlen + 10);
    snprintf(_prob->_opstr, _strlen + 10, "%s", _str);

    return 0;
}

parser_t octonumparser = {._func = octoparsenum, ._name = "nums"};
parser_t octoopsparser = {._func = octoparseops, ._name = "ops"};