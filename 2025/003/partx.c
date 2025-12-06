#include "partx.h"

DLL_NODE_CTOR(joltage_t, joltage_ctor);

bool has_same_rating(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_rating == *(size_t *)prop;
}

bool came_after(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_order > *(size_t *)prop;
}

bool did_not_come_after(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_order <= *(size_t *)prop;
}

dll_node_h pick_by_low_order(dll_node_h arga, dll_node_h argb)
{
    joltage_h _jolta = (joltage_h)arga;
    joltage_h _joltb = (joltage_h)argb;
    return _jolta->_order > _joltb->_order ? &_joltb->_node : &_jolta->_node;
}

dll_node_h comparebyrating(dll_node_h arga, dll_node_h argb)
{
    joltage_h _jolta = (joltage_h)arga;
    joltage_h _joltb = (joltage_h)argb;
    return _jolta->_rating > _joltb->_rating ? &_jolta->_node : &_joltb->_node;
}

void populatejoltages(context_h _ctx, char *_str)
{
    _ctx->_originlen = strlen(strtok(_str, "\n"));

    dll_head_init(&_ctx->_originbyrating);
    dll_head_init(&_ctx->_originbyorder);

    size_t rating = 0;
    for (size_t _ii = 0; _ii < _ctx->_originlen; _ii++)
    {
        if (1 == sscanf(_str + _ii, "%1lu", &rating))
        {
            joltage_h _njoltorder = joltage_ctor();
            _njoltorder->_rating = rating;
            _njoltorder->_order = _ii;

            joltage_h _njoltrating = joltage_ctor();
            _njoltrating->_rating = rating;
            _njoltrating->_order = _ii;

            _njoltrating->_other = _njoltorder;
            _njoltorder->_other = _njoltrating;

            dll_node_sorted_insert(&_ctx->_originbyrating, &_njoltrating->_node, comparebyrating);
            dll_node_append(&_ctx->_originbyorder, &_njoltorder->_node);
        }
    }
}

#define PREV(_n) ((joltage_h)(_n->_node._prev))
#define NEXT(_n) ((joltage_h)(_n->_node._next))

joltage_h pickearliestwithhighestbyrating(joltage_h _start)
{  
    joltage_h _selected = NULL;
    joltage_h _candidate = NEXT(_start);

    while (_candidate)
    {
        if (_candidate->_rating == _start->_rating)
        {
            if (!_selected)
                _selected = _candidate;
            else if (_candidate->_order < _selected->_order)
                _selected = _candidate;
            _candidate = NEXT(_candidate);
        }
        else
            _candidate = NULL;
    }
    return _selected ? _selected : _start;
}

joltage_h picknextelligiblebyrating(context_h _ctx)
{
    joltage_h _lastbyrating = _ctx->_pickedinlist[_ctx->_lastpickedind];
    joltage_h _nextjolt = (joltage_h)_lastbyrating->_node._prev;

    joltage_h _selected = NULL;
    joltage_h _candidate = _lastbyrating;

    while (_candidate)
    {
        if (_candidate->_order > _lastbyrating->_order)
        {
            if (!_selected)
                _selected = _candidate;
            else if (_candidate->_order < _selected->_order)
                _selected = _candidate;
        }
        _candidate = PREV(_candidate);
    }

    if (_selected)
        return _selected;

    _selected = NULL;
    _candidate = (joltage_h)_ctx->_originbyrating._first;

    while (!_selected && _candidate)
    {
        if (_candidate->_order > _lastbyrating->_order)
        {
            if (!_selected)
                _selected = _candidate;
        }
        _candidate = NEXT(_candidate);
    }
    return _selected;
}

joltage_h picknextelligiblebyorder(context_h _ctx)
{
    joltage_h _lastbyrating = _ctx->_pickedinlist[_ctx->_lastpickedind];
    joltage_h _frombyorder = (joltage_h)(_lastbyrating->_other);
    joltage_h _nextjolt = (joltage_h)(_frombyorder->_node._next);

    while (_nextjolt)
    {
        if (_nextjolt->_order > _lastbyrating->_order)
            break;
        _nextjolt = (joltage_h)_nextjolt->_node._next;
    }
    return _nextjolt;
}