#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct
{
    struct dll_node _node;
    AOC_2D_DIR dir;
    size_t steps;
} movement_t;

#define MOV_CAST(_p) ((movement_t *)_p)
#define MAP_MID_SIZE (1024)
AOC_2D_DIR char_to_AOC_2D_DIR(char *_c);