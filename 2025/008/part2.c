#include "partx.h"

typedef context_t *context_h;

#define CTX_CAST(_p) ((context_h)_p)

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    TRY_TYPE_MALLOC(_blk->_data, struct context);
    if (!_blk->_data)
        return ENOMEM;
    context_h ctx = CTX_CAST(_blk->_data);
    parser_append(&ctx->_parsers, &float3Dparser, ctx);

    ctx->_segments = RSIZE;
    ctx->_result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);
    parse_all(&ctx->_parsers, _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);
    ctx->_longestdistance = p3ddist(&ctx->_highest->_data, &ctx->_lowest->_data);
    ctx->_distancesegment = ctx->_longestdistance / ARRAY_DIM(ctx->_distances);
    ctx->_z_segment = (ctx->_highest->_data._x - ctx->_lowest->_data._x) / (1.0f * ctx->_segments);

    aoc_info("-- longuest distance: %f distance segment size: %f",
             ctx->_longestdistance,
             ctx->_distancesegment);
    aoc_info("-- points are sorted by slices of z boundaries of %f", ctx->_z_segment);

    dispatchpoints(ctx);
    distancessearchedscan(ctx, false);
    size_t missing = scanregions(ctx, false);

    /**
     * TODO : This does not really answer all cases.  
     * */
    ctx->_result = connect_x_unconnectedpoints(ctx, 21, false); /*6606*/
    getallcircuitssizes2(ctx, false);

    if (missing)
        aoc_alarm("-- not measured         %3.1lu", missing);

    aoc_ans("AOC %s %s solution is %lu", CONFIG_YEAR, _blk->_name, ctx->_result);
    return 0;
}

static void freeSolution(struct solutionCtrlBlock_t *_blk)
{
    context_h ctx = CTX_CAST(_blk->_data);

    dll_free_all(&ctx->_inputpoints, free);
    ARR_FOREACH(ind, ctx->_distances)
    {
        dll_free_all(&ctx->_distances[ind], free);
    }

    scanregionstart(head)
    {
        dll_free_all(&getregion(ctx, head), pointnode_free);
    }
    scanregionend(head);

    free(ctx->_highest);
    free(ctx->_lowest);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = freeSolution};
struct solutionCtrlBlock_t *part2 = &privPart2;

void getallcircuitssizes2(context_h ctx, bool verbose)
{
    dll_head_t circuitssizes = {0};
    dll_head_t circuit = {0};

    ARR_FOREACH(ind, ctx->_distances)
    {
        LL_FOREACH(pointaref, ctx->_distances[ind])
        {
            pointrefnode_h pointref = (pointrefnode_h)pointaref;
            if (pointref->_first_point->_scanned)
                continue;

            if (verbose)
                printf("--start-- (%p)\n", pointref->_first_point);

            getconnections(pointref->_first_point, NULL, NULL, &circuit, verbose);

            if (verbose)
            {
                size_t maxprint = 0LU;
                printf("\t\t\t\t\t\t");
                LL_FOREACH(pnode, circuit)
                {
                    voidpnode_h vpnode = (voidpnode_h)pnode;
                    float3D_h f3d = (float3D_h) & (*((pointnode_h)(vpnode->_ptr))->_f3d);
                    printf("%s ", strfloat3d(f3d, GREEN));
                    if (maxprint++ > 10)
                    {
                        printf(" ...");
                        break;
                    }
                }
                printf("\n");
            }

            dll_node_sorted_insert(&circuitssizes, intnode_ctor(circuit._size), intcomphighest);

            dll_free_all(&circuit, free);
        }
    }
    ARR_FOREACH(ind, ctx->_distances)
    {
        LL_FOREACH(pointaref, ctx->_distances[ind])
        {
            pointrefnode_h pointref = (pointrefnode_h)pointaref;
            pointref->_first_point->_scanned = false;
            reinitconnections(pointref->_first_point, NULL, NULL, &circuit, verbose);
        }
    }

    if (verbose)
        printf("--end-- \n");

    printf("-- there are %s%lu " RESET "circuit left\n",
           circuitssizes._size == 1 ? BGREEN : BRED,
           circuitssizes._size);
    printf("-- this circuit has %lu points\n", ((intnode_h)(circuitssizes._first))->_int);

    dll_free_all(&circuitssizes, free);
}