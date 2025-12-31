#include "partx.h"

AOC_2D_DIR char_to_AOC_2D_DIR(char *_c)
{
    switch (*_c)
    {
    case 'R':
        return AOC_2D_DIR_RIGHT;
    case 'U':
        return AOC_2D_DIR_UP;
    case 'L':
        return AOC_2D_DIR_LEFT;
    case 'D':
        return AOC_2D_DIR_DOWN;
    default:
        return AOC_2D_DIR_MAX;
    }
}
