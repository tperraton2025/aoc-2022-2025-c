#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context_t
{
    int result;
};

#define CTX_CAST(_p) ((struct context_t *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct context_t));
    CTX_CAST(_blk->_data)->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    char yield, his, mine;
    int points[] = {0, 3, 6};
    char *names[] = {"rock", "paper", "scissors"};

    if (2 == sscanf(_blk->_str, "%c %c\n", &his, &yield))
    {
        his -= 'A';
        yield -= 'X';
        mine = abs((his + (yield - 1)) % 3);
        if (his > 2 || mine > 2 || yield > 2)
        {
            aoc_err("%s:%d  his:%u mine:%u yield:%u", __FILE__, __LINE__, (uint8_t)his, (uint8_t)mine, (uint8_t)yield);
            exit(EXIT_FAILURE);
        }

        int round = (mine + 1) + points[yield];
        CTX_CAST(_blk->_data)->result += round;
    }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    return CTX_CAST(_blk->_data)->result;
}

static struct solutionCtrlBlock_t privPart2 = {._name = CONFIG_DAY " part 2", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue};
struct solutionCtrlBlock_t *part2 = &privPart2;