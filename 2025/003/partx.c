#include "partx.h"

DLL_NODE_CTOR(joltage_t, joltage_ctor);

bool did_not_come_after(void *arg, void *prop)
{
    joltage_h _jolta = (joltage_h)arg;
    return _jolta->_order <= *(size_t *)prop;
}

dll_node_h comparebyratingthenorder(dll_node_h arga, dll_node_h argb)
{
    joltage_h _jolta = (joltage_h)arga;
    joltage_h _joltb = (joltage_h)argb;
    if (_jolta->_rating == _joltb->_rating)
        return _jolta->_order < _joltb->_order ? &_jolta->_node : &_joltb->_node;
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

            dll_node_sorted_insert(&_ctx->_originbyrating, &_njoltrating->_node, comparebyratingthenorder);
            dll_node_append(&_ctx->_originbyorder, &_njoltorder->_node);
        }
    }
}

void freejoltchoice(void *arg)
{
    joltchoice_h choice = (joltchoice_h)arg;
    dll_free_all(choice->_availablebyrating, free);
    FREE(choice->_availablebyrating);
    FREE(choice->_conversion);
}

joltchoice_h joltagetreechoice(joltchoice_h parent, size_t decimals, dll_head_h byrating, dll_head_h byorder, joltage_h selected, void free(void *arg))
{
    joltchoice_h newchoice;
    if (!selected || !free)
        return NULL;

    TRY_TYPE_MALLOC(newchoice, joltchoice_t);
    if (!newchoice)
        return NULL;

    newchoice->_conversion = base_10_conversion(0);
    newchoice->_selected = selected;

    if (parent)
    {
        newchoice->_decimalsneeded = parent->_decimalsneeded - 1;
        newchoice->_decimalscurrent = parent->_decimalscurrent + 1;

        memcpy(newchoice->_conversion, parent->_conversion, sizeof(base10_t));

        newchoice->_availablebyrating = dll_clone_trimmed(parent->_availablebyrating, sizeof(joltage_t), (void *)&selected->_order, did_not_come_after);
        newchoice->_originbyorder = parent->_originbyorder;
    }
    else
    {
        /*  no parents, selected is the joltage linked list head with size */
        newchoice->_decimalscurrent = 0;
        newchoice->_decimalsneeded = decimals;

        newchoice->_availablebyrating = dll_clone(byrating, sizeof(joltage_t));
        newchoice->_originbyorder = byorder;
    }

    aoc_tree_node(parent ? &parent->_treenode : NULL, &newchoice->_treenode, free);
    newchoice->_treenode._free = free;
    assert(newchoice->_decimalsneeded < 64);
    newchoice->_conversion->_digits[newchoice->_decimalsneeded] = (char)selected->_rating;
    base_10_reverse_conversion(newchoice->_conversion);

    return newchoice;
}

size_t joltagepickallfeasible(joltchoice_h parent)
{
    dll_node_h _node = &parent->_selected->_node;
    joltage_h _parentjolt = parent->_selected;

    if (!parent->_decimalsneeded)
        return 0;
    if (parent->_availablebyrating->_size < parent->_decimalsneeded)
        return 0;

    LL_FOREACH_P_EXT(_node, parent->_availablebyrating)
    {
        joltage_h _nxtjoltage = (joltage_h)_node;
        if (parent->_originbyorder->_size - _nxtjoltage->_order < parent->_decimalsneeded)
            continue;

        if (_nxtjoltage->_order <= _parentjolt->_order)
            continue;

        joltchoice_h newchoice = joltagetreechoice(parent, 0, parent->_availablebyrating, parent->_originbyorder, _nxtjoltage, parent->_treenode._free);
        if (!newchoice->_decimalsneeded)
        {
            return newchoice->_conversion->_val;
        }
        size_t ret = joltagepickallfeasible(newchoice);
        if (ret)
        {
            return ret;
        }
    }
    return 0;
}

static char _strcomb[2048] = {0};
char *strcomb(joltchoice_h choice)
{
    struct dll_head path = {0};

    joltchoice_h firstchx = choice;
    while (firstchx)
    {
        voidnode_h step = malloc(sizeof(voidnode_t));
        step->_data = (void *)firstchx->_selected;
        dll_node_append(&path, &step->_node);
        firstchx = (joltchoice_h)firstchx->_treenode._parent;
    }
    voidnode_h pathnode = (voidnode_h)path._last;
    joltage_h lastchx = (joltage_h)pathnode->_data;

    char *_pen = &_strcomb[0];

    LL_FOREACH_P(_node, choice->_originbyorder)
    {
        assert((_pen - _strcomb < sizeof(_strcomb)));
        joltage_h jolth = (joltage_h)_node;

        if (lastchx)
        {
            if (lastchx->_order == jolth->_order)
            {
                _pen += sprintf(_pen, GREEN "%lu" RESET, jolth->_rating);
                pathnode = (voidnode_h)pathnode->_node._prev;
                lastchx = pathnode ? (joltage_h)pathnode->_data : NULL;
            }
            else
                _pen += sprintf(_pen, RED "%lu" RESET, jolth->_rating);
        }
        else
            _pen += sprintf(_pen, RED "%lu" RESET, jolth->_rating);
    }
    assert((_pen - _strcomb < sizeof(_strcomb)));

    dll_free_all(&path, free);
    return _strcomb;
}
