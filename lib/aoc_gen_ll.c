#include "ll_types.h"
#include "ll_items.h"
#include "ll_defines.h"
#include <stdlib.h>

dll_node_h voidpnode_ctor(void *ptr)
{
    voidpnode_t *ret = NULL;
    ret = calloc(1LU, sizeof(voidpnode_t));
    ret->_ptr = ptr;
    return &ret->_node;
}

float3D_t float3D_ctor(float x, float y, float z);

intb10node_h intb10node_ctor()
{
    intb10node_h _ret = malloc(sizeof(intb10node_t));
    return _ret;
}

void intb10node_free(void *arg)
{
    intb10node_h _ret = (intb10node_h)arg;
    free(_ret->_int);
    free(arg);
}

stringnode_h stringnode_ctor(const char *const name)
{
    stringnode_h _ret = malloc(sizeof(stringnode_t));
    if (!_ret)
        return NULL;
    _ret->_str = malloc(strnlen(name, MAX_STR_LEN) + 1);
    if (!_ret->_str)
        goto abort;

    strcpy(_ret->_str, name);
    return _ret;

abort:
    FREE(_ret);
    return NULL;
}

void stringnode_free(void *string)
{
    stringnode_h _stringh = (stringnode_h)string;
    FREE(_stringh->_str);
    FREE(_stringh);
}

dll_node_h stringnode_compare(dll_node_h _a, dll_node_h _b)
{
    stringnode_h _string_a = (stringnode_h)_a;
    stringnode_h _string_b = (stringnode_h)_b;
    int _ret = strncmp(_string_a->_str, _string_b->_str, MAX_STR_LEN);
    if (_ret >= 0)
        return _b;
    else
        return _a;
}

int stringnode_rename(stringnode_h node, const char *const name)
{
    char *_nname = malloc(strnlen(name, MAX_STR_LEN) + 1);
    if (!_nname)
        return ENOMEM;
    FREE(node->_str);
    snprintf(_nname, strnlen(name, MAX_STR_LEN) + 1, "%s", name);
    node->_str = _nname;
    return 0;
}

dll_node_h floatnode_ctor(float val)
{
    floatnode_h ret;
    ret = calloc(1LU, sizeof(floatnode_t));
    ret->_val = val;
    return &ret->_node;
}

float3D_t float3D_ctor(const float x, const float y, const float z)
{
    float3D_t val = {._x = x, ._y = y, ._z = z};
    return val;
}

dll_node_h float3Dnode_ctor(const float3D_t const data)
{
    float3Dnode_h val = NULL;
    val = calloc(1LU, sizeof(float3Dnode_t));
    val->_data = data;
    return &val->_node;
}

dll_node_h intnode_ctor(size_t value)
{
    intnode_h ret = NULL;
    ret = calloc(1LU, sizeof(intnode_t));
    ret->_int = value;
    return &ret->_node;
}

dll_node_h intcomphighest(dll_node_h arga, dll_node_h argb)
{
    return ((intnode_h)arga)->_int > ((intnode_h)argb)->_int ? arga : argb;
}

dll_node_h voidparrnode_ctor(void **data, size_t len)
{
    voidparrnode_h new = NULL;
    new = calloc(1LU, sizeof(voidparrnode_t));
    new->_len = len;
    memcpy(&new->_ptr, data, len * sizeof(void *));
    return &new->_node;
}