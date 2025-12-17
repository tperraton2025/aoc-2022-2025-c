#include "partx.h"

int posparser(void *arg, char *str)
{
    context_h ctx = (context_h)arg;
    if (2 == sscanf(str, "%lu,%lu\n", &(ctx->_cursor._x), &(ctx->_cursor._y)))
    {
        aoc_2d_obj_h _nobj = aoc_2d_obj_ctor(ctx->_eng, "#", &ctx->_cursor, "#", OBJ_FLAG_STATIC, "");
        aoc_2d_eng_append_obj(ctx->_eng, _nobj);
    }
    return 0;
}
parser_t blockparser = {._func = posparser, ._name = "blockparser"};

int scanboundaries(context_h ctx){
    
}