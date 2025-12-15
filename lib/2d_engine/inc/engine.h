#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ansi_cmd.h>
#include <aoc_linked_list.h>
#include "objects.h"
#include "aoc_2d_engine.h"
#include "aoc_ranges.h"
#include "aoc_types.h"

int move_within_coord(aoc_2d_eng_h eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);

typedef struct objmap
{
    aoc_2d_obj_h **_map;
    size_t _objcount;
    size_t _cellcountx;
    size_t _cellcounty;
} objmap_t;

typedef objmap_t *objmap_h;

struct ascii_2d_engine
{
    coordpair_t _drawlimits;
    coordpair_t _poslim;
    dll_compare *_objsort;
    coord_t _partoffset;
    size_t _newlinecnt;
    coord_t _cursor;
    char _voidsym;
    dll_head_t _objects;
    objmap_t _objmap;
    size_t _statCol;
    size_t _statColOffset;
    size_t _statLine;
    size_t _PrivStatLine;
    bool _drawingenabled;
    bool _prevenabledraw;
    bool _collisionsenabled;
    bool _collisionmapenabled;
    size_t _delay;
};

int allocmap(aoc_2d_eng_h eng);
void freemap(aoc_2d_eng_h eng);

int aoc_2d_eng_extend_drawing_area(struct ascii_2d_engine *eng, coord_t _ns);
int is_position_in_box(aoc_2d_eng_h eng, coord_t *_pos);

int move_cursor_until(struct ascii_2d_engine *eng, AOC_2D_DIR _dir, size_t _steps, coord_t *limit);

void aoc_2d_eng_erase_stats(aoc_2d_eng_h eng);
int aoc_2d_eng_draw_box(struct ascii_2d_engine *eng);

int put_pos(aoc_2d_eng_h eng, coord_t *_pos, coord_t *_npos);
int engine_fill_hv_line(struct ascii_2d_engine *eng, coord_t *_start, coord_t *_end, AOC_2D_DIR _dir, const char *_c);
int move_within_window(aoc_2d_eng_h eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);
int engine_cursor_private_next_stats(struct ascii_2d_engine *eng);
int engine_fill_drawing_area(struct ascii_2d_engine *eng);
int engine_cursor_log(struct ascii_2d_engine *eng);
int engine_cursor_stats(struct ascii_2d_engine *eng);

int aoc_2d_obj_ctor_fit_detect(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, size_t steps, AOC_2D_DIR dir);
int aoc_2d_eng_calculate_obj_position(aoc_2d_obj_h obj);

part_h aoc_2d_eng_get_part_by_position(aoc_2d_eng_h eng, coord_t *pos);

dll_node_h pickhighestY(dll_node_h arga, dll_node_h argb);

static const coordpair_t _drawlimits = {._max = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y},
                                        ._min = {._x = 1, ._y = 1}};
static const coordpair_t _coordpair = {._max = {._x = ABSOLUTE_MAX_X, ._y = ABSOLUTE_MAX_Y},
                                       ._min = {._x = 0, ._y = 0}};