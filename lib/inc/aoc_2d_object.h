#ifndef AOC_2D_OBJ_H
#define AOC_2D_OBJ_H

#include "aoc_coordinates.h"
#include "aoc_linked_list.h"

#define OBJH(_objref) ((aoc_2d_obj_h)((aoc_2d_obj_ref_h)_objref)->data)

typedef struct object *aoc_2d_obj_h;
typedef struct aoc_2d_obj_ref
{
    struct dll_node _node;
    aoc_2d_obj_h data;
    aoc_2d_obj_h parent;
    int _blocked;
} aoc_2d_obj_ref_t;

typedef struct aoc_2d_obj_ref *aoc_2d_obj_ref_h;

const coord_t *const aoc_2d_obj_get_pos(aoc_2d_obj_h _obj);
const char *const aoc_2d_obj_name(aoc_2d_obj_h _obj);
aoc_2d_obj_ref_h aoc_2d_obj_ref_ctor(aoc_2d_obj_h _obj);
int aoc_2d_obj_increment_collision_counter(aoc_2d_obj_h _obj);
size_t aoc_2d_obj_get_collisioncounter(aoc_2d_obj_h _obj);
const char *const aoc_2d_obj_get_name(aoc_2d_obj_h _obj);

#endif