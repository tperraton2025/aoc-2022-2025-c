#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "aoc_helpers.h"

void signal_int_handler(int sig)
{
}

int aocSolution(struct solutionCtrlBlock_t *_sol, int argc, char *argv[])
{
    int ret = 0;

    clock_t cstart = clock();

    assert(_sol && "NULL solutionCtrlBlk provided by user");
    assert(_sol->_name && "NULL solution name provided by user");
    assert(_sol->_handler && "NULL _handler function provided by user");
    assert(_sol->_prologue && "NULL _prologue function provided by user");
    assert(_sol->_epilogue && "NULL _epilogue function provided by user");

    FILE *_pxfile = fopen(argv[1], "r");
    if (_pxfile == NULL)
    {
        aoc_err("%s:%d: Error opening %s!", __func__, __LINE__, argv[1]);
        exit(EXIT_FAILURE);
    }

    char _ins[MAX_LINE_LEN] = {0};

    ret = _sol->_prologue(_sol, argc, argv);

    while (!ret && fgets(_ins, sizeof(_ins), _pxfile))
    {
        _sol->_str = _ins;
        ret = _sol->_handler(_sol);
        if (ret)
            break;
    }

    if (!ret)
        ret = _sol->_epilogue(_sol);

    fclose(_pxfile);

    _sol->_free(_sol);

    clock_t cend = clock();
    clock_t timelapse = cend - cstart;
    double _ms = timelapse / (double)(CLOCKS_PER_SEC / 1000);
    aoc_info("returned %s in %4.3fms", strerror(ret), _ms);

    return ret;
}

int aocFileLessSolution(struct solutionCtrlBlock_t *_sol, int argc, char *argv[], char **input)
{
    int ret = 0;

    clock_t start = clock();

    assert(_sol && "NULL solutionCtrlBlk provided by user");
    assert(_sol->_name && "NULL solution name provided by user");
    assert(_sol->_handler && "NULL _handler function provided by user");
    assert(_sol->_prologue && "NULL _prologue function provided by user");
    assert(_sol->_epilogue && "NULL _epilogue function provided by user");

    if (argv[1] == NULL)
    {
        aoc_err("%s", "error finding input data");
        exit(EXIT_FAILURE);
    }

    char _ins[MAX_LINE_LEN] = {0};

    ret = _sol->_prologue(_sol, argc, argv);

    size_t _it = 0;
    while (!ret && input[_it])
    {
        _sol->_str = input[_it++];
        ret = _sol->_handler(_sol);
        if (ret)
            break;
    }

    if (!ret)
        ret = _sol->_epilogue(_sol);

    _sol->_free(_sol);

    clock_t end = clock();
    clock_t timelapse = end - start;

    double _ms = timelapse / (double)(CLOCKS_PER_SEC / 1000);
    aoc_info("returned %s in %4.3fms", strerror(ret), _ms);

    return ret;
}
