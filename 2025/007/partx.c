#include "partx.h"

int objectsparser(void *arg, char *str)
{
    context_h _ctx = (context_h)arg;
    _ctx->_cursor._x = 0;

    char *_symlist[] = {"^", "S"};

    ARR_FOREACH(_it, _symlist)
    {
        _ctx->_cursor._y;
        char *_srcsym = _symlist[_it];
        char *_start = str;
        char *_strpi = str;

        _strpi = strchr(str, *_srcsym);
        while (_strpi)
        {
            _ctx->_cursor._x = _strpi - _start;
            aoc_2d_obj_h _nobj = aoc_2d_obj_ctor(_ctx->_eng, _srcsym, &_ctx->_cursor, _srcsym, OBJ_FLAG_STATIC, "");
            aoc_2d_eng_append_obj(_ctx->_eng, _nobj);
            _strpi = strchr(_strpi + 1, *_srcsym);
        }
        _ctx->_cursor._y++;
    }
    return 0;
}

parser_t _objparser = {._name = "objectsparser", ._func = objectsparser};

int initpropagation(context_h ctx)
{
    aoc_2d_eng_h eng = ctx->_eng;
    aoc_2d_obj_h start = aoc_2d_eng_get_obj_by_name(eng, "S");
    if (!start)
        return EINVAL;

    ctx->_cursor = aoc_2d_eng_get_obj_position(eng, start);
    ctx->_cursor._y++;

    aoc_2d_obj_h beam = aoc_2d_obj_ctor(eng, "beam", &ctx->_cursor, "|", OBJ_FLAG_MOBILE | OBJ_FLAG_TRACEING, "");
    aoc_2d_eng_append_obj(eng, beam);
    ctx->_created++;
    aoc_2d_obj_ref_h beamref = aoc_2d_obj_ref(beam);
    dll_node_append(&ctx->_activebeams, &beamref->_node);

    return 0;
}

int loopbeampropagations(context_h ctx)
{
    aoc_2d_eng_h eng = ctx->_eng;
    int ret = 0;
    int active = 1;

    size_t p = sizeof(size_t *);
    dll_node_t ghostnode = {0};
    while (active)
    {
        LL_FOREACH(beamnode, ctx->_activebeams)
        {
            active = 0;
            if (beamnode->_obsolete)
                continue;
            active = 1;
            aoc_2d_obj_h originbeam = (aoc_2d_obj_h)((aoc_2d_obj_ref_h)beamnode)->data;
            ctx->_cursor = aoc_2d_eng_get_obj_position(eng, originbeam);

            coord_t prevcoord = ctx->_cursor;
            int mvret = aoc_2d_eng_step_obj(eng, originbeam, 1, AOC_2D_DIR_DOWN, GREEN);
            while (!mvret)
            {
                coord_t prevcoord = ctx->_cursor;
                mvret = aoc_2d_eng_step_obj(eng, originbeam, 1, AOC_2D_DIR_DOWN, GREEN);
            }
            if (EEXIST == mvret)
            {
                prevcoord._y += 1;
                aoc_2d_obj_h objunder = aoc_2d_eng_get_obj_by_position(eng, &prevcoord);
                if (objunder)
                {
                    if (!strcmp(aoc_2d_eng_get_obj_name(objunder), "^"))
                    {
                        beamnode->_obsolete = true;
                        ctx->_result++;
                        coord_t _postoaddbeamsat[] = {{._x = ctx->_cursor._x - 1,
                                                       ._y = ctx->_cursor._y + 1},
                                                      {._x = ctx->_cursor._x + 1,
                                                       ._y = ctx->_cursor._y + 1}};

                        ghostnode._next = NULL;
                        ARR_FOREACH(_it, _postoaddbeamsat)
                        {
                            aoc_2d_obj_h nbeam = aoc_2d_obj_ctor(eng, "beam", &_postoaddbeamsat[_it], "|", OBJ_FLAG_MOBILE | OBJ_FLAG_TRACEING, "");
                            if (!nbeam)
                                continue;
                            ctx->_created++;
                            aoc_2d_eng_append_obj(eng, nbeam);
                            aoc_2d_obj_ref_h nbeamref = aoc_2d_obj_ref(nbeam);

                            if (!ghostnode._next)
                                ghostnode._next = &nbeamref->_node;
                            if (beamnode == ctx->_activebeams._last)
                                beamnode = &ghostnode;
                            dll_node_append(&ctx->_activebeams, &nbeamref->_node);
                        }
                    }
                }
            }
            else if (ERANGE == mvret)
            {
                beamnode->_obsolete = true;
                aoc_2d_eng_obj_delete(eng, originbeam);

                ghostnode._next = beamnode->_next;
                dll_node_disconnect(&ctx->_activebeams, beamnode);
                free(beamnode);
                if (!ghostnode._next)
                    break;
                beamnode = &ghostnode;
            }
        }
        aoc_2d_eng_exit_drawing_area(eng);
    }
    return ret;
}

splitter_h splitter(splitter_h parent, aoc_2d_eng_h eng, coord_t *position)
{
    splitter_h _splitter = NULL;
    TRY_RAII_MALLOC(_splitter, sizeof(splitter_t));
    if (!_splitter)
        return NULL;

    _splitter->_position._x = position->_x;
    _splitter->_position._y = position->_y;

    _splitter->_eng = eng;
    return _splitter;
}

void splitter_findchildren(splitter_h splitter) {
    
}
void splitter_findtimeimpact(splitter_h splitter) {}