#include "ll_types.h"
#include "ll_items.h"
#include "ll_defines.h"
#include <stdlib.h>

DLL_NODE_CTOR(intlistitem_t, intlistnode_ctor);

float3D_t float3D_ctor(float x, float y, float z);

float3Dnode_h float3Dnode_ctor(const float3D_t const data);

intb10item_h intb10item_ctor()
{
    intb10item_h _ret = malloc(sizeof(intb10item_t));
    return _ret;
}

void intb10item_free(void *arg)
{
    intb10item_h _ret = (intb10item_h)arg;
    free(_ret->_int);
    free(arg);
}

stringnode_h string_dll(const char *const name)
{
    stringnode_h _ret = malloc(sizeof(stringnode_t));
    if (!_ret)
        return NULL;
    _ret->_str = malloc(strnlen(name, MAX_STR_DLL_LEN) + 1);
    if (!_ret->_str)
        goto abort;

    strcpy(_ret->_str, name);
    return _ret;

abort:
    FREE(_ret);
    return NULL;
}

void string_dll_free(void *string)
{
    stringnode_h _stringh = (stringnode_h)string;
    FREE(_stringh->_str);
    FREE(_stringh);
}

dll_node_h string_dll_compare(dll_node_h _a, dll_node_h _b)
{
    stringnode_h _string_a = (stringnode_h)_a;
    stringnode_h _string_b = (stringnode_h)_b;
    int _ret = strcmp(_string_a->_str, _string_b->_str);
    if (_ret >= 0)
        return _b;
    else
        return _a;
}

int string_dll_rename(stringnode_h node, const char *const name)
{
    char *_nname = malloc(strnlen(name, MAX_STR_DLL_LEN) + 1);
    if (!_nname)
        return ENOMEM;
    FREE(node->_str);
    sprintf(_nname, "%s", name);
    node->_str = _nname;
    return 0;
}

floatnode_h floatnode_ctor(float val)
{
    floatnode_h ret;
    TRY_TYPE_MALLOC(ret, floatnode_t);
    ret->_val = val;
    return ret;
}

float3D_t float3D_ctor(const float x, const float y, const float z)
{
    float3D_t val = {._x = x, ._y = y, ._z = z};
    return val;
}

float3Dnode_h float3Dnode_ctor(const float3D_t const data)
{
    float3Dnode_h val = NULL;
    TRY_TYPE_MALLOC(val, float3Dnode_t);
    val->_data = data;
    return val;
}