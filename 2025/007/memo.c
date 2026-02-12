#include "partx.h"

memospawn_h memospawn_ctor(aoc_2d_obj_h splitter)
{
    memospawn_h ret = calloc(1LU, sizeof(memospawn_t));
    ret->_splitter = splitter;
    if (!splitter)
        return ret;
    const coord_t *const underpos = aoc_2d_obj_get_pos(splitter);
    ret->_spwncord[0]._x = underpos->_x - 1;
    ret->_spwncord[0]._y = underpos->_y;
    ret->_spwncord[1]._x = underpos->_x + 1;
    ret->_spwncord[1]._y = underpos->_y;
    return ret;
}

void freememo(memospawn_h memo)
{
    dll_free_all(&memo->_from, free);
    free(memo);
}

memospawn_h lookupmemo(memospawn_h **memomap, coord_t *pos)
{
    return memomap[pos->_x][pos->_y];
}

void addmemoentry(context_h ctx, aoc_2d_obj_ref_h beamref, aoc_2d_obj_h newsplt)
{
    coord_t newsplt_p = *aoc_2d_obj_get_pos(newsplt);

    memospawn_h **memap = ctx->_memosplits;
    memospawn_h *nsplit_mem = &memap[newsplt_p._x][newsplt_p._y];

    if (!(*nsplit_mem))
    {
        *nsplit_mem = memospawn_ctor(newsplt);
    }

    if (!beamref)
        return;
    if (!beamref->parent)
        return;

    coord_t parent_p = *aoc_2d_obj_get_pos(beamref->parent);
    memospawn_h *psplit_mem = &memap[parent_p._x][parent_p._y];
    ARR_FOREACH(spawns_it, (*psplit_mem)->_subsplitters)
    {
        if (!(*psplit_mem)->_subsplitters[spawns_it])
        {
            (*psplit_mem)->_subsplitters[spawns_it] = newsplt;
            break;
        }
    }
}

sizepair_t memoreport(context_h ctx, aoc_2d_obj_h top_splt_h, size_t progr)
{
    sizepair_t ans = {._0 = 0LU, ._1 = 1LU};

    aoc_2d_eng_h eng = (aoc_2d_eng_h)ctx->_eng;
    aoc_2d_eng_draw(eng);

    memospawn_h **memomap = ctx->_memosplits;
    coord_t top_splt_p = *aoc_2d_obj_get_pos(top_splt_h);
    memospawn_h top_splt_mem = *getmemoentry(ctx, top_splt_h);

    ARR_FOREACH(btm_splt_it, top_splt_mem->_subsplitters)
    {
        aoc_2d_obj_h btm_spl_h = top_splt_mem->_subsplitters[btm_splt_it];
        if (!btm_spl_h)
            continue;
        memospawn_h btm_splt_mem = *getmemoentry(ctx, btm_spl_h);
        if (!btm_splt_mem)
            continue;

        size_t hits = aoc_2d_obj_get_collisioncounter(btm_spl_h);
        size_t btm_timeline = btm_splt_mem->_timelines;
        ans._1 += (hits ? 1LU : 0LU) * btm_timeline;
    }

    LL_FOREACH_P(spln, ctx->_allspliters)
    {
        aoc_2d_obj_ref_h spltref = (aoc_2d_obj_ref_h)spln;
        aoc_2d_obj_h btm_spl_h = (aoc_2d_obj_h)spltref->data;
        size_t hits = aoc_2d_obj_get_collisioncounter(btm_spl_h);
        if (hits)
            ans._0++;

        size_t splttl = 0;
        memospawn_h btm_splt_mem = (*getmemoentry(ctx, btm_spl_h));
        if (btm_splt_mem)
            splttl = btm_splt_mem->_timelines;

        coord_t btm_spl_p = *aoc_2d_obj_get_pos(btm_spl_h);
        char numstr[16] = "X";
        if (ctx->_drawrequest)
        {
            if (top_splt_h == btm_spl_h)
                snprintf(numstr,sizeof(numstr), BGREEN "%c", 'S');
            else
                snprintf(numstr,sizeof(numstr), "%s%*.1lu", splttl ? GREEN : YELLOW, 1, splttl);
            if (splttl > 10LU)
                btm_spl_p._x--;
            btm_spl_p._y--;
            aoc_2d_eng_draw_sym_at(eng, &btm_spl_p, numstr, "");
            btm_spl_p._y++;
        }
    }

    (*getmemoentry(ctx, top_splt_h))->_timelines += ans._1;
    ans._1 = memomap[top_splt_p._x][top_splt_p._y]->_timelines;
    if (ctx->_drawrequest)
        aoc_info("[%4lu/%4lu]: %s: %s impacts %lu splitters and adds %lu timelines",
                 progr,
                 ctx->_allspliters->_size,
                 aoc_2d_obj_name(top_splt_h),
                 strpos(&top_splt_p),
                 ans._0,
                 ans._1);
    return ans;
}

static size_t display = 0;
static size_t result = 1;

memospawn_h *const getmemoentry(context_h ctx, aoc_2d_obj_h obj)
{
    coord_t pos = *aoc_2d_obj_get_pos(obj);
    return &ctx->_memosplits[pos._x][pos._y];
}
