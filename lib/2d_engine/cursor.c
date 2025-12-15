#include "aoc_helpers.h"
#include "engine.h"
#include <assert.h>

int move_cursor_until(struct ascii_2d_engine *eng, AOC_2D_DIR _dir, size_t _steps, coord_t *limit)
{
    int ret = 0;
    switch (_dir)
    {
    case AOC_2D_DIR_UP:
        if (eng->_cursor._y == limit->_y)
            return ERANGE;
        eng->_cursor._y -= _steps;
        break;
    case AOC_2D_DIR_DOWN:
        if ((eng->_cursor._y + _steps) > limit->_y)
            return ERANGE;
        eng->_cursor._y += _steps;
        break;
    case AOC_2D_DIR_LEFT:
        if (eng->_cursor._x == limit->_x)
            return ERANGE;
        eng->_cursor._x -= _steps;
        break;
    case AOC_2D_DIR_RIGHT:
        if ((eng->_cursor._x + _steps) > limit->_x)
            return ERANGE;
        eng->_cursor._x += _steps;
        break;

    default:
        return EINVAL;
    }
    return ret;
}

int move_within_coord(aoc_2d_eng_h eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir)
{
    switch (_dir)
    {
    case AOC_2D_DIR_UP:
        if (!N_BETWEEN_AB(_pos->_y - _steps, eng->_poslim._min._y, eng->_poslim._max._y))
            return ERANGE;
        _pos->_y -= _steps;
        break;
    case AOC_2D_DIR_DOWN:
        if (!N_BETWEEN_AB(_pos->_y + _steps, eng->_poslim._min._y, eng->_poslim._max._y))
            return ERANGE;
        _pos->_y += _steps;
        break;
    case AOC_2D_DIR_LEFT:
        if (!N_BETWEEN_AB(_pos->_x - _steps, eng->_poslim._min._x, eng->_poslim._max._x))
            return ERANGE;
        _pos->_x -= _steps;
        break;
    case AOC_2D_DIR_RIGHT:
        if (!N_BETWEEN_AB(_pos->_x + _steps, eng->_poslim._min._x, eng->_poslim._max._x))
            return ERANGE;
        _pos->_x += _steps;
        break;
    default:
        return EINVAL;
    }
    return 0;
}

int move_within_window(aoc_2d_eng_h eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir)
{
    switch (_dir)
    {
    case AOC_2D_DIR_UP:
        if (!N_BETWEEN_AB(_pos->_y - _steps, eng->_drawlimits._min._y, eng->_drawlimits._max._y))
            return ERANGE;
        _pos->_y -= _steps;
        break;
    case AOC_2D_DIR_DOWN:
        if (!N_BETWEEN_AB(_pos->_y + _steps, eng->_drawlimits._min._y, eng->_drawlimits._max._y))
            return ERANGE;
        _pos->_y += _steps;
        break;
    case AOC_2D_DIR_LEFT:
        if (!N_BETWEEN_AB(_pos->_x - _steps, eng->_drawlimits._min._x, eng->_drawlimits._max._x))
            return ERANGE;
        _pos->_x -= _steps;
        break;
    case AOC_2D_DIR_RIGHT:
        if (!N_BETWEEN_AB(_pos->_x + _steps, eng->_drawlimits._min._x, eng->_drawlimits._max._x))
            return ERANGE;
        _pos->_x += _steps;
        break;

    default:
        return EINVAL;
    }
    return 0;
}

int is_position_in_box(aoc_2d_eng_h eng, coord_t *_pos)
{
    if (!N_BETWEEN_AB(_pos->_y, eng->_poslim._min._y, eng->_poslim._max._y))
        return ERANGE;
    if (!N_BETWEEN_AB(_pos->_x, eng->_poslim._min._x, eng->_poslim._max._x))
        return ERANGE;
    return 0;
}

int put_pos(aoc_2d_eng_h eng, coord_t *_pos, coord_t *_npos)
{
    int _ret = is_position_in_box(eng, _npos);
    if (_ret)
        return _ret;

    _pos->_y = _npos->_y;
    _pos->_x = _npos->_x;

    return 0;
}

int put_part(aoc_2d_eng_h eng, part_h _sym, coord_t *_delta)
{
    if (!N_BETWEEN_AB(_delta->_y, eng->_poslim._min._x, eng->_poslim._max._y))
        return ERANGE;
    if (!N_BETWEEN_AB(_delta->_x, eng->_poslim._min._x, eng->_poslim._max._x))
        return ERANGE;
    return 0;
}

int engine_put_cursor_in_footer_area(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    if (eng->_drawingenabled)
    {
        printf(MCUR_FMT ERASE_LINE_FROM_CR, eng->_drawlimits._max._y + 1LU, 1LU);
    }
    return 0;
}

int engine_cursor_log(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    printf(MCUR_FMT ERASE_LINE_FROM_CR, eng->_drawlimits._max._y + 2LU, 0LU);
    return 0;
}

int engine_cursor_stats(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    eng->_statLine = 2;
    eng->_statCol = 0;
    if (eng->_drawingenabled)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, eng->_statLine++, eng->_drawlimits._max._x + 2 + (eng->_statCol * eng->_statColOffset));
    return 0;
}

int engine_cursor_user_stats(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    eng->_statLine = eng->_PrivStatLine;
    eng->_statCol = 0;
    if (eng->_drawingenabled)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, eng->_statLine++, eng->_drawlimits._max._x + 2 + (eng->_statCol * eng->_statColOffset));
    return 0;
}

int engine_cursor_private_next_stats(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    if (eng->_drawingenabled)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, eng->_statLine++, eng->_drawlimits._max._x + 2 + (eng->_statCol * eng->_statColOffset));
    eng->_PrivStatLine = eng->_statLine + 1;
    return 0;
}

int engine_cursor_user_next_stats(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    if (eng->_drawingenabled)
        printf(MCUR_FMT ERASE_LINE_FROM_CR, eng->_statLine++, eng->_drawlimits._max._x + 2 + (eng->_statCol * eng->_statColOffset));
    if (eng->_statLine >= eng->_drawlimits._max._y)
    {
        eng->_statLine = eng->_PrivStatLine;
        eng->_statCol++;
        return ERANGE;
    }
    return 0;
}

int aoc_2d_eng_exit_drawing_area(struct ascii_2d_engine *eng)
{
    if (!eng)
        return EINVAL;
    if (eng->_drawingenabled)
        printf(MCUR_FMT ERASE_LINE_FROM_CR "\n", eng->_drawlimits._max._y + 10, 0LU);
    return 0;
}
