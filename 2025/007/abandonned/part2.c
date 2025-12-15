#include "partx.h"

typedef struct splitter
{
    tree_node_t _treenode;
    aoc_2d_eng_h _eng;
    coord_tracker_t _position;
    size_t _splitonleft;
    size_t _splitonright;
    size_t _impacts;
} splitter_t;
typedef splitter_t *splitter_h;

splitter_h newsplitter(splitter_h parent, aoc_2d_eng_h eng, coord_t *position);
splitter_h splitter_findorigin(aoc_2d_eng_h eng);
bool isparentsplitter(void *_a, void *_b);
void splitter_findchildren(splitter_h splitter);
void splitter_findtimeimpact(splitter_h splitter);
size_t splitter_showfinalimpacts(splitter_h splitter);

/**
 *
 * This solution was abandonned as it did not yet find a way to
 * merge parent/child relations with the splitters of same
 * coordinates,
 * resulted in excessive memory cost and extremely low performances;
 *
 */
static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    TRY_RAII_MALLOC(_blk->_data, sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    context_h _ctx = CTX_CAST(_blk->_data);
    _ctx->_result = 0;

    coord_t _lim = {._x = 2, ._y = 2};
    _ctx->_eng = aoc_2d_eng_create(&_lim, '.', 10, pickhighestY, false);

    parser_append(&_ctx->_parsers, &_objparser, _ctx);
    aoc_2d_eng_parse_cli(_ctx->_eng, argc, argv);

    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    parse_all(&_ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    aoc_2d_eng_h eng = _ctx->_eng;
    int ret = 0;

    aoc_2d_obj_h start = aoc_2d_eng_get_obj_by_name(eng, "S");
    aoc_2d_eng_obj_delete(eng, start);

    aoc_2d_eng_extend_one_direction(eng, 2, AOC_2D_DIR_RIGHT);
    aoc_2d_eng_extend_one_direction(eng, 2, AOC_2D_DIR_LEFT);

    aoc_2d_eng_draw(_ctx->_eng);

    _ctx->_root = splitter_findorigin(eng);
    splitter_findchildren(_ctx->_root);
    splitter_findtimeimpact(_ctx->_root);

    _ctx->_result = splitter_showfinalimpacts(_ctx->_root);

    aoc_tree_free_all(&_ctx->_root->_treenode);

    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, _ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h _ctx = CTX_CAST(_blk->_data);
    engine_free(_ctx->_eng);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;

int linesparser(void *arg, char *str)
{
    context_h _ctx = (context_h)arg;
    _ctx->_cursor._y++;
}

parser_t _objparser = {._name = "objparser", ._func = objparser};

bool ischildsplitter(void *a, void *b)
{
    dll_search_t *_search = (dll_search_t *)a;
    coord_t *_acord = &((coord_tracker_t *)_search->_arg)->_coord;
    coord_t *_bcord = &((coord_tracker_t *)b)->_coord;

    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    /** this means the other is higher in graphical representation  */
    if ((distx != 1) || (_acord->_y >= _bcord->_y))
    {
        return true;
    }
    else
    {
        coord_tracker_h newtracker = coordtracker_ctor(_bcord);
        dll_trim_nodes(&_search->_result, b, coord_equal_x_lower_y, free);
        dll_node_append(&_search->_result, &newtracker->_node);
    }
}

bool ishighestsplitter(void *a, void *b)
{
    dll_search_t *_search = (dll_search_t *)a;
    coord_t *_bcord = &((coord_tracker_t *)b)->_coord;

    /** this means lower in graphical representation  */
    if (!_search->_result._size)
    {
        coord_tracker_h newtracker = coordtracker_ctor(_bcord);
        dll_node_append(&_search->_result, &newtracker->_node);
    }
    else if (_bcord->_y < ((coord_tracker_h)_search->_result._first)->_coord._y)
    {
        coord_tracker_h newtracker = coordtracker_ctor(_bcord);
        dll_free_all(&_search->_result, free);
        dll_node_append(&_search->_result, &newtracker->_node);
    }
    return true;
}

bool isrightunder(void *a, void *b)
{
    dll_search_t *_search = (dll_search_t *)a;
    coord_t *_acord = &((coord_tracker_t *)_search->_arg)->_coord;
    coord_t *_bcord = &((coord_tracker_t *)b)->_coord;

    size_t distx = ABSDIFF(_acord->_x, _bcord->_x);
    /** this means lower in graphical representation  */
    if ((distx != 0) || (_acord->_y >= _bcord->_y))
        return true;
    else
    {
        if (!(coord_tracker_h)_search->_result._size)
        {
            coord_tracker_h newtracker = coordtracker_ctor(_bcord);
            dll_node_append(&_search->_result, &newtracker->_node);
        }
        if (_bcord->_y < ((coord_tracker_h)_search->_result._first)->_coord._y)
        {
            coord_tracker_h newtracker = coordtracker_ctor(_bcord);
            dll_free_all(&_search->_result, free);
            dll_node_append(&_search->_result, &newtracker->_node);
        }
    }
    return true;
}

bool searchsplittersarg(void *a, void *b)
{
    dll_search_t *_search = (dll_search_t *)a;
    bool _result = _search->_matchfunc(a, b);
    _search->_continue = _search->_continuefunc ? _search->_continuefunc(a, b) : true;
    return _search->_continue;
}

bool searchsplitters(aoc_2d_eng_h eng, dll_search_t *search)
{
    aoc_2d_eng_foreach_objpos_arg(eng, search, searchsplittersarg);
    if (!search->_result._size)
    {
        // aoc_warn("no result for %s", search->_name);
        return false;
    }
    // aoc_info("result for %s", search->_name);
    // aoc_info("%s", "found");
    LL_FOREACH(_result, search->_result)
    {
        coord_tracker_h _splitres = (coord_tracker_h)_result;
        // aoc_info("%s", strpos(&_splitres->_coord));
    }
    // aoc_info("%s", "end result\n");
    return true;
}

void freesplitter(void *arg)
{
}

splitter_h newsplitter(splitter_h parent, aoc_2d_eng_h eng, coord_t *position)
{
    splitter_h _splitter = NULL;
    TRY_RAII_MALLOC(_splitter, sizeof(splitter_t));
    if (!_splitter)
        return NULL;

    _splitter->_position._coord._x = position->_x;
    _splitter->_position._coord._y = position->_y;

    _splitter->_eng = eng;

    aoc_tree_node(parent ? &parent->_treenode : NULL, &_splitter->_treenode, freesplitter);
    return _splitter;
}

splitter_h splitter_findorigin(aoc_2d_eng_h eng)
{
    dll_search_t _childsearch = {._name = "origin", ._matchfunc = ishighestsplitter};
    if (!searchsplitters(eng, &_childsearch))
        return NULL;

    coord_t *_originpos = &((coord_tracker_h)_childsearch._result._first)->_coord;
    splitter_h mothersplitter = newsplitter(NULL, eng, _originpos);
    mothersplitter->_splitonleft = 1;
    mothersplitter->_splitonright = 1;
    mothersplitter->_impacts = 1;
    dll_free_all(&_childsearch._result, free);
    return mothersplitter;
}

void splitter_findchildren(splitter_h splitter)
{
    char buffer[64] = "children of";
    strcat(buffer, strpos(&splitter->_position._coord));

    dll_search_t _childsearch = {._name = buffer, ._matchfunc = ischildsplitter, ._arg = &splitter->_position};
    searchsplitters(splitter->_eng, &_childsearch);

    LL_FOREACH(_node, _childsearch._result)
    {
        splitter_h new = newsplitter(splitter, splitter->_eng, &((coord_tracker_h)_node)->_coord);

        splitter_findchildren(new);
    }
    dll_free_all(&_childsearch._result, free);
}

void splitter_findtimeimpact(splitter_h splitter)
{
    if (!splitter)
        return;

    LL_FOREACH(_node, splitter->_treenode._dllchildren)
    {
        splitter_h _childsplitter = (splitter_h)_node;

        _childsplitter->_splitonleft++;
        _childsplitter->_splitonright++;
        _childsplitter->_impacts += splitter->_impacts;
        char posa[32] = "";
        char posb[32] = "";
        strcpy(posa, strpos(&splitter->_position._coord));
        strcpy(posb, strpos(&_childsplitter->_position._coord));

        // aoc_info("%s incremented %s to %lu^%lu (%lu)", posa, posb, _childsplitter->_splitonleft, _childsplitter->_splitonright, _childsplitter->_impacts);
    }

    LL_FOREACH_EXT(_node, splitter->_treenode._dllchildren)
    {
        splitter_findtimeimpact((splitter_h)_node);
    }
    return;
}

typedef struct treescan
{
    size_t result;
    size_t impacts;
} treescan_t;

void splitter_showindividualimpacts(void *arg, tree_node_h node)
{
    treescan_t *result = (treescan_t *)arg;
    splitter_h splitter = (splitter_h)node;
    result->result += splitter->_splitonleft + splitter->_splitonright;
    result->impacts += splitter->_impacts;
    // aoc_ans("%s : %lu^%lu", strpos(&splitter->_position._coord), splitter->_splitonleft, splitter->_splitonright);
}

size_t splitter_showfinalimpacts(splitter_h splitter)
{
    treescan_t result = {0};
    aoc_tree_foreach_nodes_arg(&splitter->_treenode, &result, splitter_showindividualimpacts);
    return result.impacts + 1;
}