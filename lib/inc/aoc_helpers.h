#ifndef AOC_HELPERS
#define AOC_HELPERS

#include <stdio.h>
#include "ansi_fmt.h"
#include "aoc_types.h"
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define FREE(_p)      \
    {                 \
        if (_p)       \
            free(_p); \
        _p = NULL;    \
    }

#define TRY_TYPE_MALLOC(_p, _t)        \
    {                                  \
        _p = (_t *)malloc(sizeof(_t)); \
        if (_p)                        \
            memset(_p, 0, sizeof(_t)); \
    }

#define TRY_ARR_MALLOC(_p, _len)   \
    {                              \
        _p = malloc((_len));       \
        if (_p)                    \
            memset(_p, 0, (_len)); \
    }

#define TRY_COPY_MALLOC(_p, _s, _i) \
    {                               \
        _p = malloc(_s);            \
        if (_p)                     \
            memcpy(_p, _i, _s);     \
    }

#define ARRAY_DIM(_arr) (sizeof(_arr) / sizeof(_arr[0]))
#define CAST(_t, _p) ((_t)_p)
#define MAX_LINE_LEN (1024)

#define ARR_FOREACH(_it, _arr) for (size_t _it = 0; _it < ARRAY_DIM(_arr); _it++)
#define ARR_FOREACH_START(_it, _it0, _arr) for (size_t _it = _it0; _it < ARRAY_DIM(_arr); _it++)

/* array of usize max size are deemed not possible */
#define ARR_FOREACH_REV(_it, _arr) for (size_t _it = ARRAY_DIM(_arr) - 1; _it < __SIZE_MAX__; _it--)
#define RANGE_FOR(_it, _start, _end) for (size_t _it = _start; _it < _end; _it++)

#define STR_BUFF_LEN(_str) (sizeof(_str))
#define STR_BUFFER(name, default) static char name[] = default;

#define aoc_info(_fmt, ...) printf(_fmt "\r\n", __VA_ARGS__);
#define aoc_alarm(_fmt, ...) printf(RED "!!!!\t" _fmt "\t!!!!\r\n" RESET, __VA_ARGS__);
#define aoc_err(_fmt, ...) printf(RED _fmt RESET "\r\n", __VA_ARGS__);
#define aoc_warn(_fmt, ...) printf(YELLOW _fmt RESET "\r\n", __VA_ARGS__);
#define aoc_ans(_fmt, ...)                             \
    {                                                  \
        printf(BGREEN _fmt RESET "\r\n", __VA_ARGS__); \
    }
#define aoc_prompt(_fmt, ...)           \
    {                                   \
        printf(ERASE_FULL_LINE);        \
        printf(_fmt "\r", __VA_ARGS__); \
    }

#define block()                                              \
    {                                                        \
        aoc_info("%s %s:%i", "blocked", __FILE__, __LINE__); \
        scanf("%*c");                                        \
    }

struct solutionCtrlBlock_t
{
    char *_name;
    char *_str;
    int (*_prologue)(struct solutionCtrlBlock_t *_data, int argc, char *argv[]);
    int (*_handler)(struct solutionCtrlBlock_t *_data);
    int (*_epilogue)(struct solutionCtrlBlock_t *_data);
    void (*_free)(struct solutionCtrlBlock_t *_data);
    void *_data;
};

/**
 * Takes path to input txt file and ensures its validity
 * It then handles solving the challenge by calling _func
 * with each line of the file successfully opened until
 * it reaches its end;
 *
 * @param _inPath   : path to file
 * @param _func     : int returning function pointer
 */
extern int aocSolution(struct solutionCtrlBlock_t *_sol, int argc, char *argv[]);

/*  when I am too tired to parse, like 2025 day 006 */
int aocFileLessSolution(struct solutionCtrlBlock_t *_sol, int argc, char *argv[], char **input);

#endif