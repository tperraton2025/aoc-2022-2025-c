#include <aoc_helpers.h>
#include <aoc_linked_list.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
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
    char mine, his;
    int points[] = {0, 3, 6};
    char *names[] = {"rock", "paper", "scissors"};

    if (2 == sscanf(_blk->_str, "%c %c\n", &his, &mine))
    {
        his -= 'A';
        mine -= 'X';
        size_t index = abs((1 + mine - his) % 3);
        if (his > 2 || mine > 2 || index > 2)
        {
            aoc_err("%s:%d his:%u mine:%u index:%lu", __FILE__, __LINE__, (uint8_t)his, (uint8_t)mine, index);
            exit(EXIT_FAILURE);
        }
        int round = (mine + 1) + points[index];
        CTX_CAST(_blk->_data)->result += round;
    }
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    return CTX_CAST(_blk->_data)->result;
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue};
struct solutionCtrlBlock_t *part1 = &privPart1;