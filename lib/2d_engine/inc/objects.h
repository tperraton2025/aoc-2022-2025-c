#ifndef AOC_PRIV_OBJECTS_H
#define AOC_PRIV_OBJECTS_H

#include "aoc_linked_list.h"
#include "aoc_coordinates.h"
#include "aoc_2d_object.h"

#define MAX_NAME_LEN_LUI (16LU)
#define MAX_NAME_LEN_STR "16"
#define OBJ_NAME_FMT "%" MAX_NAME_LEN_STR "s"

#define ABSOLUTE_MAX_PART_CNT (32)
#define ABSOLUTE_MAX_PART_FMT_LEN (16)

typedef struct object
{
    struct dll_node _node;
    dll_head_t _parts;
    coord_t _pos;
    char *_name;
    char *_fmt;
    size_t _props;
    void *_userdata;
    size_t _refcnt;
    size_t _collisionscounter;
} object_t;
typedef object_t *object_h;

typedef struct part
{
    sll_node_h _node;
    struct object *_parent;
    coord_t _pos;
    char _sym;
    char *_fmt;
} part_t;

typedef struct part *part_h;

part_h aoc_2d_eng_new_part(struct object *obj, coord_t *rpos, char sym, char *fmt);
void aoc_2d_eng_free_part(void *_data);

#endif