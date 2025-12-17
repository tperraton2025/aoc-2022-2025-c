#ifndef AOC_2D_ENGINE_H
#define AOC_2D_ENGINE_H

#include <errno.h>
#include "aoc_types.h"
#include "ansi_cmd.h"
#include "aoc_ranges.h"
#include "aoc_coordinates.h"
#include "aoc_2d_object.h"
#include "aoc_linked_list.h"

#define ABSOLUTE_MAX_X (100000000)
#define ABSOLUTE_MAX_Y (100000000)

#define DRAWABLE_MAX_X (64)
#define DRAWABLE_MAX_Y (128)

#define ABSOLUTE_MAX_STAT_LEN (128)
#define ABSOLUTE_MAX_STATS_LINES (128)

#define COORD_RANGE_CHECK(_coord, _range) (N_BETWEEN_AB(_coord._x, _range._min._x, _range._max._x) && N_BETWEEN_AB(_coord._y, _range._min._y, _range._max._y))
#define COORD_RANGE_CHECK_P(_coord, _range) (N_BETWEEN_AB(_coord->_x, _range._min._x, _range._max._x) && N_BETWEEN_AB(_coord->_y, _range._min._y, _range._max._y))

#define COORD_MAXIMA_CHECK(_a, _b) (_a._x <= _b._x) && (_a._y <= _b._y)
#define BOUNDARY_CHECK_P(_a, _b) (_a->_x <= _b->_x) && (_a->_y <= _b->_y)

#define SYM_CAST(_p) ((part_h)_p)
#define aoc_2d_eng_prompt_multistr(eng, _sleep, ...)                                 \
    {                                                                                \
        const char *_str[] = {__VA_ARGS__};                                          \
        aoc_2d_eng_prompt(eng, _sleep, sizeof(_str) / sizeof(_str[0]), __VA_ARGS__); \
    }

#define aoc_2d_eng_prompt_extra_stats_as_err(eng, _fmt, ...)         \
    {                                                                \
        engine_cursor_user_next_stats(eng);                          \
        printf(ERASE_LINE_FROM_CR RED _fmt RESET "\n", __VA_ARGS__); \
    }

typedef enum
{
    OBJ_PROPERTY_STATIC = 0,
    OBJ_PROPERTY_MOBILE,
    OBJ_PROPERTY_START,
    OBJ_PROPERTY_END,
    OBJ_PROPERTY_NO_COLLISION,
    OBJ_PROPERTY_TRACEING,
    OBJ_PROPERTY_MAX
} object_properties_t;

typedef enum
{
    OBJ_FLAG_STATIC = (1 << OBJ_PROPERTY_STATIC),
    OBJ_FLAG_MOBILE = (1 << OBJ_PROPERTY_MOBILE),
    OBJ_FLAG_START = (1 << OBJ_PROPERTY_START),
    OBJ_FLAG_END = (1 << OBJ_PROPERTY_END),
    OBJ_FLAG_NO_COLLISION = (1 << OBJ_PROPERTY_NO_COLLISION),
    OBJ_FLAG_TRACEING = (1 << OBJ_PROPERTY_TRACEING),
    OBJ_FLAG_MAX
} object_flags_t;

typedef struct ascii_2d_engine *aoc_2d_eng_h;

void aoc_2d_eng_suspend_draw(aoc_2d_eng_h eng);
void aoc_2d_eng_unsuspend_draw(aoc_2d_eng_h eng);
int aoc_2d_eng_erase_obj(struct ascii_2d_engine *eng, struct object *obj);
void aoc_2d_eng_erase_stats(aoc_2d_eng_h eng);

aoc_2d_eng_h aoc_2d_eng_create(coord_t *partcoordmaxima, char _voidsym, size_t delay, dll_compare *comp, bool collisionmap);
void aoc_2d_obj_delete(aoc_2d_eng_h eng, struct object *obj);
void aoc_2d_eng_free_obj(void *_data);
void eng_set_refresh_delay(aoc_2d_eng_h eng, size_t delay);
int aoc_2d_eng_draw_objects(aoc_2d_eng_h eng);
int aoc_2d_eng_draw_obj(struct ascii_2d_engine *eng, struct object *obj, char *specfmt);

int aoc_2d_eng_extend_one_direction(aoc_2d_eng_h eng, size_t steps, AOC_2D_DIR _dir);
void engine_free(aoc_2d_eng_h eng);
int aoc_2d_eng_draw(aoc_2d_eng_h eng);

aoc_2d_obj_h aoc_2d_obj_ctor(aoc_2d_eng_h eng, const char *const name, coord_t *pos, char *sym, size_t props, const char *const fmt);
int aoc_2d_eng_append_obj(aoc_2d_eng_h eng, aoc_2d_obj_h _obj);
aoc_2d_obj_h aoc_2d_eng_get_obj_by_name(aoc_2d_eng_h eng, const char *name);
aoc_2d_obj_h aoc_2d_eng_get_obj_by_position(aoc_2d_eng_h eng, coord_t *_pos);
size_t aoc_2d_eng_get_obj_count(aoc_2d_eng_h eng);

int aoc_2d_eng_step_obj(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, size_t steps, AOC_2D_DIR dir, char *_fmt);
int aoc_2d_eng_put_obj_and_redraw(aoc_2d_eng_h eng, aoc_2d_obj_h _obj, coord_t _npos);

coordpair_t aoc_2d_eng_get_parts_boundaries(aoc_2d_eng_h eng);
size_t aoc_2d_eng_get_XY_moves_between_objects(aoc_2d_eng_h eng, aoc_2d_obj_h _a, aoc_2d_obj_h _b);
size_t aoc_2d_eng_get_XYD_moves_between_objects(aoc_2d_eng_h eng, aoc_2d_obj_h _a, aoc_2d_obj_h _b);

void aoc_2d_eng_prompt_obj_list(aoc_2d_eng_h eng);
int engine_put_cursor_in_footer_area(struct ascii_2d_engine *eng);
int engine_cursor_user_next_stats(struct ascii_2d_engine *eng);
int engine_cursor_user_stats(struct ascii_2d_engine *eng);

int aoc_2d_eng_exit_drawing_area(struct ascii_2d_engine *eng);

int aoc_2d_eng_draw_part_at(aoc_2d_eng_h eng, coord_t *_pos, char *_sym, const char *fmt);
int aoc_2d_eng_draw_symbol_at(aoc_2d_eng_h eng, coord_t *_pos, const char *_sym, const char *fmt);
size_t aoc_2d_eng_get_dist_between_objects(aoc_2d_eng_h eng, aoc_2d_obj_h _a, aoc_2d_obj_h _b);
int aoc_2d_eng_move_one_step_towards(aoc_2d_eng_h eng, aoc_2d_obj_h _a, coord_t _pos);

void engine_clear_screen();
void aoc_2d_eng_enable_draw(aoc_2d_eng_h eng);
void aoc_2d_eng_disable_draw(aoc_2d_eng_h eng);
int move_within_window(aoc_2d_eng_h eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);

int aoc_inputs_ansi_to_dir(const char *const _str, AOC_2D_DIR *_dir);
void aoc_2d_eng_prompt(aoc_2d_eng_h eng, const size_t _sleep, size_t _count, ...);

void aoc_2d_eng_foreach_objpos_arg(aoc_2d_eng_h eng, void *arg, bool func(void *arga, void *argb));
void aoc_2d_eng_set_obj_flag(aoc_2d_obj_h obj, size_t flag);
void aoc_2d_eng_clear_obj_flag(aoc_2d_obj_h obj, size_t flag);

int move_within_coord(aoc_2d_eng_h eng, coord_t *_pos, size_t _steps, AOC_2D_DIR _dir);

void aoc_2d_obj_ref_free(void *arg);
void aoc_2d_eng_prompt_stats(aoc_2d_eng_h eng);
dll_head_t *aoc_2d_eng_prompt_obj_list_with_a_part_at_position(aoc_2d_eng_h eng, coord_t *_pos);

char *strobjcnt(aoc_2d_eng_h eng);

void engine_add_line(aoc_2d_eng_h eng);
size_t engine_last_line(aoc_2d_eng_h eng);

/**
 * listing API
 */
dll_head_h aoc_2d_eng_list_obj_pos(aoc_2d_eng_h eng, dll_compare *comp);
dll_head_h aoc_2d_eng_list_obj_pos_arg(aoc_2d_eng_h eng, void *arg, bool func(void *arga, void *argb));
dll_head_h aoc_2d_eng_list_objects(aoc_2d_eng_h eng, const char *const incl, const char *const excl);

void aoc_2d_eng_get_canvas_center(aoc_2d_eng_h eng, coord_t *_pos);

/**
 * Collisions management
 *
 */
size_t aoc_2d_eng_get_total_of_collisions(aoc_2d_eng_h eng);
int aoc_2d_eng_collision_at(aoc_2d_eng_h eng, aoc_2d_obj_h excl, coord_t *_pos);
int aoc_2d_eng_clear_all_collision_counters(aoc_2d_eng_h eng);
int engine_remove_obj(aoc_2d_eng_h eng, aoc_2d_obj_h obj);
void aoc_2d_eng_parse_cli(aoc_2d_eng_h eng, int argc, char **argv);

bool aoc_2d_eng_isdrawing(aoc_2d_eng_h eng);
#endif