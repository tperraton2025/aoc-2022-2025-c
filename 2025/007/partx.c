#include "partx.h"

int objectsparser(void *arg, char *str)
{
    context_h ctx = (context_h)arg;
    ctx->_cursor._x = 0;

    char *_symlist[] = {"^", "S"};

    ARR_FOREACH(_it, _symlist)
    {
        ctx->_cursor._y;
        char *_srcsym = _symlist[_it];
        char *_start = str;
        char *_strpi = str;

        _strpi = strchr(str, *_srcsym);
        while (_strpi)
        {
            ctx->_cursor._x = _strpi - _start;
            aoc_2d_obj_h _nobj = aoc_2d_obj_ctor(ctx->_eng, _srcsym, &ctx->_cursor, _srcsym, OBJ_FLAG_STATIC, "");
            aoc_2d_eng_append_obj(ctx->_eng, _nobj);
            _strpi = strchr(_strpi + 1, *_srcsym);
            if (_symlist[_it][0] == 'S')
                ctx->_start = _nobj;
        }
    }
    ctx->_cursor._y++;
    return 0;
}

int initbeamexploration(context_h ctx, aoc_2d_obj_h obj)
{
    aoc_2d_eng_h eng = ctx->_eng;
    ctx->_drawrequest = aoc_2d_eng_isdrawing(ctx->_eng);
    aoc_2d_eng_clear_all_collision_counters(eng);

    if (!ctx->_start)
        return EINVAL;
    ctx->_cursor = *aoc_2d_obj_get_pos(obj);

    coord_t spawnxy[] = {{._x = ctx->_cursor._x - 1, ._y = ctx->_cursor._y},
                         {._x = ctx->_cursor._x + 1, ._y = ctx->_cursor._y},
                         {._x = ctx->_cursor._x, ._y = ctx->_cursor._y + 1}};

    size_t spstart = obj == ctx->_start ? 2 : 0;
    size_t spend = obj == ctx->_start ? 3 : 2;
    for (size_t _it = spstart; _it < spend; _it++)
    {
        aoc_2d_obj_h beam = aoc_2d_obj_ctor(eng, "beam", &spawnxy[_it], "|", OBJ_FLAG_MOBILE, "");
        aoc_2d_eng_append_obj(eng, beam);
        aoc_2d_obj_ref_h beamref = aoc_2d_obj_ref_ctor(beam);
        beamref->parent = obj;
        dll_node_append(&ctx->_activebeams, &beamref->_node);
    }

    ctx->_result = 0;
    ctx->limits = aoc_2d_eng_get_parts_boundaries(eng);

    if (!ctx->_memosplits)
        DYN2DMALLOC(ctx->_memosplits, memospawn_h, ctx->limits._max._x + 1, ctx->limits._max._y + 1);

    return 0;
}

void beamsplit(context_h ctx, dll_node_h beamnode, aoc_2d_obj_h objunder)
{
    aoc_2d_eng_h eng = ctx->_eng;
    coordpair_t spawncords = {0};
    spawncords._min = *aoc_2d_obj_get_pos(objunder);
    spawncords._max = *aoc_2d_obj_get_pos(objunder);
    spawncords._max._x++;
    spawncords._min._x--;
    aoc_2d_obj_h obja = aoc_2d_obj_ctor(eng, "beam", &spawncords._max, "|", OBJ_FLAG_MOBILE, "");
    aoc_2d_obj_h objb = aoc_2d_obj_ctor(eng, "beam", &spawncords._min, "|", OBJ_FLAG_MOBILE, "");
    aoc_2d_obj_ref_h refa = aoc_2d_obj_ref_ctor(obja);
    aoc_2d_obj_ref_h refb = aoc_2d_obj_ref_ctor(objb);
    refa->parent = objunder;
    refb->parent = objunder;
    aoc_2d_eng_append_obj(eng, obja);
    aoc_2d_eng_append_obj(eng, objb);
    dll_node_insert_after(&ctx->_activebeams, &refa->_node, beamnode);
    dll_node_insert_after(&ctx->_activebeams, &refb->_node, &refa->_node);
    assert(beamnode->_next == &refa->_node);
    assert(refa->_node._next == &refb->_node);
}

aoc_2d_obj_h beamexplore(context_h ctx, aoc_2d_obj_h start)
{
    aoc_2d_eng_h eng = ctx->_eng;
    aoc_2d_obj_h firsthit = NULL;
    int ret = 0;

    ret = initbeamexploration(ctx, start);
    addmemoentry(ctx, NULL, start);
    if (ret)
        return NULL;

    int active = 1;
    bool explored = false;
    size_t display = 0;

    while (active)
    {
        active = 0;

        dll_node_t ghostnode = {0};
        LL_FOREACH(beamnode, ctx->_activebeams)
        {
            aoc_2d_obj_h beam = OBJH(beamnode);
            aoc_2d_obj_h split = beamfindsplit(eng, beam);
            if (!firsthit)
                firsthit = split;
            if (split)
            {
                addmemoentry(ctx, (aoc_2d_obj_ref_h)beamnode, split);
                if (1LU == aoc_2d_obj_get_collisioncounter(split))
                {
                    beamsplit(ctx, beamnode, split);
                }
            }

            dll_node_disconnect(&ctx->_activebeams, beamnode);
            assert(!strcmp(aoc_2d_obj_get_name(beam), "beam"));
            aoc_2d_obj_delete(eng, beam);
            ghostnode._next = beamnode->_next;
            free(beamnode);
            beamnode = &ghostnode;
        }
    }
    return firsthit;
}

size_t beamexploit(context_h ctx, aoc_2d_obj_h obj)
{
    coord_t start = *aoc_2d_obj_get_pos(obj);
    size_t progr = 0;

    for (size_t iix = 0; iix < ctx->limits._max._x; iix++)
    {
        for (size_t iiy = 0; iiy < ctx->limits._max._y; iiy++)
        {
            memospawn_h memo = ctx->_memosplits[iix][iiy];
            if (memo)
            {
                progr++;
                aoc_info("[%4lu/%4lu]: %s: %s hits %lu splitters",
                         progr,
                         ctx->_allspliters->_size,
                         aoc_2d_obj_name(memo->_splitter),
                         strpos(aoc_2d_obj_get_pos(memo->_splitter)),
                         memo->_timelines);
            }
        }
    }
}

parser_t _objparser = {._func = objectsparser,
                       ._name = "objparser"};

aoc_2d_obj_h beamfindsplit(aoc_2d_eng_h eng, aoc_2d_obj_h beam)
{
    int mvret = 0;
    aoc_2d_eng_draw_obj(eng, beam, "");
    mvret = aoc_2d_eng_step_obj(eng, beam, 1, AOC_2D_DIR_DOWN, GREEN);
    while (!mvret)
    {
        mvret = aoc_2d_eng_step_obj(eng, beam, 1, AOC_2D_DIR_DOWN, GREEN);
    }
    coord_t posatend = *aoc_2d_obj_get_pos(beam);
    posatend._y++;
    if (mvret == EEXIST)
        return aoc_2d_eng_get_obj_by_position(eng, &posatend);
    return NULL;
}