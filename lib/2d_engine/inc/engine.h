#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ansi_cmd.h>
#include <aoc_linked_list.h>
#include "aoc_2d_engine.h"
#include "aoc_ranges.h"
#include "aoc_types.h"

typedef struct object
{
    struct dll_node _node;
    struct dll_head _parts;
    coord_t _pos;
    char *_name;
    size_t _props;
} object_t;

typedef struct object *object_h;

typedef struct part
{
    struct dll_node _node;
    object_h _parent;
    coord_t _pos;
    char _sym;
    char *_fmt;
} part_t;

typedef struct part *part_h;

part_h eng_part_create(struct object *obj, coord_t *rpos, char sym, char *fmt);
int engine_draw_part_at(aoc_2d_engine_h _eng, coord_t *_pos, char *_sym);
int move_within_box(aoc_2d_engine_h _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);
void aoc_engine_free_part(void *_data);

struct ascii_2d_engine
{
    coordboundaries_t _drawboundaries;
    coordboundaries_t _partboundaries;
    coord_t _cursor;
    char _voidsym;
    struct dll_head _objects;
    size_t _statLine;
    size_t _PrivStatLine;
    bool _enabledraw;
    size_t _delay;
};

int engine_extend_drawing_area(struct ascii_2d_engine *_eng, coord_t _ns); 
int is_position_in_box(aoc_2d_engine_h _eng, coord_t *_pos);
int put_pos(aoc_2d_engine_h _eng, coord_t *_pos, coord_t *_npos);

int engine_fill_hv_line(struct ascii_2d_engine *_eng, coord_t *_start, coord_t *_end, AOC_2D_DIR _dir, const char *_c);
int move_cursor_until(struct ascii_2d_engine *_eng, AOC_2D_DIR _dir, size_t _steps, coord_t *limit);

int aoc_engine_erase_object(struct ascii_2d_engine *eng, struct object *obj);

int move_within_window(aoc_2d_engine_h _eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);
void aoc_engine_prompt_stats(aoc_2d_engine_h _eng);


int engine_draw_box(struct ascii_2d_engine *_eng);
int engine_cursor_private_next_stats(struct ascii_2d_engine *_eng);
int engine_fill_drawing_area(struct ascii_2d_engine *_eng);
int engine_cursor_log(struct ascii_2d_engine *_eng);
int engine_cursor_stats(struct ascii_2d_engine *_eng);

int aoc_engine_object_fit_detect(aoc_2d_engine_h _eng, aoc_2d_object_h _obj, size_t steps, AOC_2D_DIR dir);
int aoc_engine_calculate_object_position(aoc_2d_object_h obj);

part_h aoc_engine_get_part_by_position(aoc_2d_engine_h eng, coord_t *pos);