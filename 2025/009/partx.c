#include "partx.h"

int posparser(void *arg, char *str)
{
    context_h ctx = (context_h)arg;
    if (2 == sscanf(str, "%lu,%lu\n", &(ctx->_cursor._x), &(ctx->_cursor._y)))
    {
        aoc_2d_obj_h _nobj = aoc_2d_obj_ctor(ctx->_eng, "#", &ctx->_cursor, "#", OBJ_FLAG_STATIC, "");
        aoc_2d_eng_append_obj(ctx->_eng, _nobj);
    }
    return 0;
}
parser_t blockparser = {._func = posparser, ._name = "blockparser"};

#define GETPOS(dllnode) (((coordnode_h)dllnode)->_coord)

int scanboundaries(context_h ctx)
{
    populateedges(ctx, AXIS_X);
    // populateedges(ctx, AXIS_Y);
}

int trackcoord(coord_t *goal, coord_t *curs)
{
    if (curs->_x != goal->_x)
        curs->_x = curs->_x < goal->_x ? (curs->_x + 1LU) : (curs->_x ? curs->_x - 1 : 0LU);
    if (curs->_y != goal->_y)
        curs->_y = curs->_y < goal->_y ? (curs->_y + 1LU) : (curs->_y ? curs->_y - 1 : 0LU);
    return (goal->_x == curs->_x) && (goal->_y == curs->_y);
}

dll_node_h stopatlastonsame_axis(dll_node_h start, size_t axislabel, size_t axisval)
{
    dll_node_h lastonsamecol = start;
    while (lastonsamecol->_next)
    {
        if (((size_t *)&GETPOS(lastonsamecol->_next))[axislabel] == axisval)
            lastonsamecol = lastonsamecol->_next;
        else
            break;
    }
    return lastonsamecol;
}

void get_axis_edgesfromslice(dll_node_h start, dll_node_h end, size_t axis, coordpair_h thr)
{
    for (dll_node_h nodeslice = start; nodeslice != end->_next; nodeslice = nodeslice->_next)
    {
        coord_t pos = GETPOS(nodeslice);
        if (((size_t *)&pos)[axis] > ((size_t *)&thr->_max)[axis])
            thr->_max = pos;
        if (((size_t *)&pos)[axis] < ((size_t *)&thr->_min)[axis])
            thr->_min = pos;
    }
}

void appendthr(dll_head_h head, coordpair_h thr, const char *const name, bool verbose)
{
    dll_node_append(head, coordnode_ctor(&thr->_max));
    dll_node_append(head, coordnode_ctor(&thr->_min));
    if (verbose)
        printf(YELLOW "-- %s threshold : added %s\n" RESET, name, strpospair(thr));
}

void populateedges(context_h const ctx, const axis_t axis)
{
    coordpair_t thr = {0};

    assert(axis < AXIS_MAX);

    const axis_t const_axis = axis;
    const axis_t varia_axis = AXIS_X == axis ? AXIS_Y : AXIS_X;

    const dll_head_h blockslists[] = {ctx->_blocks_xaligned, ctx->_blocks_yaligned};
    const char *colors[] = {RED, BLUE};
    const char *name[] = {"X", "Y"};

    LL_FOREACH_P(node, blockslists[const_axis])
    {
        coord_t pos = GETPOS(node);
        /** gets a slice of same alignment */
        dll_node_h lastonaxis = stopatlastonsame_axis(node, const_axis, ((size_t *)&pos)[const_axis]);

        ((size_t *)&thr._max)[varia_axis] = 0LU;
        ((size_t *)&thr._min)[varia_axis] = SIZE_MAX;

        if (ctx->_verbose)
        {
            printf("-- %s threshold : scanning from %s", name[axis], strpos(&pos));
            printf(" to %s\n", strpos(&GETPOS(lastonaxis)));
        }

        get_axis_edgesfromslice(node, lastonaxis, varia_axis, &thr);

        if (((size_t *)&thr._max)[varia_axis] > ((size_t *)&thr._min)[varia_axis])
            appendthr(&ctx->_boundaries, &thr, name[axis], ctx->_verbose);

        node = lastonaxis;
        if (!node)
            break;
    }

    dll_free_all(blockslists[const_axis], free);

    LL_FOREACH_EXT(node, ctx->_boundaries)
    {
        coordnode_h coordn = (coordnode_h)node;
        dll_node_append(blockslists[const_axis], coordnode_ctor(&coordn->_coord));
        aoc_2d_eng_draw_sym_at(ctx->_eng, &((coordnode_h)node)->_coord, "*", colors[axis]);
    }

    dll_free_all(&ctx->_boundaries, free);
}

void mergeboundaries(context_h ctx)
{
    dll_node_h pointxsorted = ctx->_blocks_xaligned->_first;
    dll_node_h pointysorted = NULL;

    while (pointxsorted)
    {
        dll_node_append(&ctx->_boundaries, coordnode_ctor(&GETPOS(pointxsorted)));
        printf("-- plot %s \n", strpos(&GETPOS(pointxsorted)));
        if (pointxsorted->_next)
        {
            dll_node_append(&ctx->_boundaries, coordnode_ctor(&GETPOS(pointxsorted->_next)));
            printf("-- plot %s \n", strpos(&GETPOS(pointxsorted->_next)));
        }

        /**
         * using y aligned edges to get the vertical next point
         * */

        pointysorted = dll_node_find_by_property(
            ctx->_blocks_yaligned,
            pointxsorted,
            coordnodes_equal);

        if (pointysorted->_next)
        {
            printf("-- tryplot %s \n", strpos(&GETPOS(pointysorted->_next)));
            if (GETPOS(pointysorted->_next)._y != GETPOS(pointxsorted)._y)
            {
                if (pointysorted->_prev)
                {
                    printf("-- tryplot %s \n", strpos(&GETPOS(pointysorted->_next)));
                    pointxsorted = dll_node_find_by_property(
                        ctx->_blocks_yaligned,
                        &GETPOS(pointysorted->_prev),
                        coordnodes_equal);
                }
            }
            else
                pointxsorted = dll_node_find_by_property(
                    ctx->_blocks_yaligned,
                    &GETPOS(pointysorted->_next),
                    coordnodes_equal);
        }
    }
}

void plotboundaries(context_h ctx)
{
    LL_FOREACH(node, ctx->_boundaries)
    {
        ctx->_cursor = GETPOS(node);
        do
        {
            printf(ERASE_LINE_FROM_CR "-- plotted " GREEN " %s" RESET "\n", strpos(&ctx->_cursor));
            aoc_2d_eng_draw_sym_at(ctx->_eng, &ctx->_cursor, "+", coordt_equal(&ctx->_cursor, &GETPOS(node)) ? RED : GREEN);
        } while (!trackcoord(&GETPOS((node->_next ? node->_next : ctx->_boundaries._first)), &ctx->_cursor));
        if (ctx->_verbose)
            printf(ERASE_LINE_FROM_CR "-- plotted " GREEN " %s" RESET "\n", strpos(&GETPOS(node)));
    }
}
