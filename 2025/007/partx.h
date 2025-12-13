#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <aoc_2d_dynarr.h>
#include <aoc_2d_engine.h>
#include <aoc_parser.h>
#include <aoc_tree.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/**
 *  - Mapped my spawn coordinates in member     memospawn_h **_memosplits;
 *  - gives spawns coordinatess in                coord_t spawns[2];
 *
 *
 */
typedef struct memospawn
{
    aoc_2d_obj_h _splitter;
    coord_t _spwncord[2];
    aoc_2d_obj_h _subsplitters[2];
    struct dll_head _from;
    size_t _timelines;
    size_t _hits;
    bool _fallsdown;
} memospawn_t;
typedef memospawn_t *memospawn_h;

memospawn_h memospawn_ctor();
void freememo(memospawn_h memo);

typedef struct context
{
    aoc_2d_eng_h _eng;
    struct dll_head _parsers;
    struct dll_head _activebeams;
    memospawn_h **_memosplits;
    memospawn_h _memohit;
    coordpair_t limits;

    aoc_2d_obj_h _start;
    coord_t _cursor;
    size_t _result;
    bool _drawrequest;
    bool _recursive;
    dll_head_h _allobjs;
    dll_head_h _allspliters;
} context_t;

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

int objectsparser(void *arga, char *str);
int initbeamexploration(context_h ctx, aoc_2d_obj_h start);

aoc_2d_obj_h beamfindsplit(aoc_2d_eng_h eng, aoc_2d_obj_h beam);
aoc_2d_obj_h beamexplore(context_h ctx, aoc_2d_obj_h start);
size_t beamexploit(context_h ctx, aoc_2d_obj_h obj);
void beamsplit(context_h ctx, dll_node_h beamnode, aoc_2d_obj_h objunder);
 
memospawn_h *const getmemoentry(context_h ctx, aoc_2d_obj_h obj);
void addmemoentry(context_h ctx, aoc_2d_obj_ref_h beamref, aoc_2d_obj_h newsplt);

void newmemorecursion(memospawn_h memo, memospawn_h **memomap);
size_t memoreport(context_h ctx, aoc_2d_obj_h start, size_t progr);
extern parser_t _objparser;
