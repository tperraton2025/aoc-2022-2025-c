#include <aoc_helpers.h>
#include "partx.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct context
{
    aoc_parser_h _parsers[3];
    size_t _usedmem;
    fdir_t *_cdir;
    fdir_t _root;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)

static int prologue(struct solutionCtrlBlock_t *_blk)
{
    _blk->_data = malloc(sizeof(struct context));
    struct context *_ctx = CTX_CAST(_blk->_data);

    dir("/", &_ctx->_root);
    _ctx->_cdir->path = _ctx->_root.path;

    _ctx->_parsers[0] = &_cmd_parser;
    _ctx->_parsers[2] = &_dir_parser;
    _ctx->_parsers[1] = &_file_parser;

    _ctx->_usedmem = 0;
    _ctx->result = 0;
    return 0;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    parse_all(CTX_CAST(_blk->_data), _blk->_str);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    int result = CTX_CAST(_blk->_data)->result;
    aoc_ans("AOC 2022 %s solution is %d", _blk->_name, result);
    return result;
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CAST(struct context *, _blk->_data);
    free(_blk->_data);
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;

int command_parser(aoc_context_h _ctx, char *_str)
{
    int match = 0;
    char _cmd[16] = {0};
    char _arg[16] = {0};

    if (sscanf(_str, "$ %15s %15s", _cmd, _arg) > 0)
    {
        if (0 == strncmp("cd", _cmd, 15))
        {
            aoc_tree_node_h _npwd = NULL;
            if (0 == strncmp("..", _arg, 15))
                _npwd = _ctx->_cdir->path._parent;
            if (!_npwd)
                _npwd = aoc_tree_search_node_by_property(&_ctx->_cdir->path, _arg, find_dir_by_name);
            if (!_npwd)
                _npwd = aoc_tree_search_node_by_property(&_ctx->_root.path, _arg, find_dir_by_name);
            if (_npwd)
                _ctx->_cdir = (fdir_t *)_npwd;
            else
                aoc_err("dir %s not found, staying in current dir", _arg);
        }
        else if (0 == strncmp("ls", _cmd, 15))
        {
            pwd(&_ctx->_cdir->path);
        }
        return 0;
    }
    return EINVAL;
}

int file_parser(aoc_context_h _ctx, char *_str)
{
    int match = 0;
    size_t _nfsize = 0;
    char _fullname[NAME_MAX_LEN] = {0};
    char _fname[16] = {0};
    char _fext[16] = {0};

    if (3 == sscanf(_str, "%ld %s.%s", &_nfsize, _fname, _fext))
    {
        char *_pname = _fullname;
        _pname = strcat(_fullname, _fname);
        _pname = strcat(_pname, ".");
        _pname = strcat(_pname, _fext);
        file_t *_nfile = file(_fullname, _nfsize, _ctx->_cdir);
        if (_nfile)
            if (dll_node_append(&_ctx->_cdir->_content, &_nfile->dir->path._llnode))
            {
                return 0;
            }
            else
                delete_file(_nfile);
        else
            return ENOMEM;
        return 0;
    }
    return EINVAL;
}

int dir_parser(aoc_context_h _ctx, char *_str)
{
    int match = 0;
    size_t _nfsize = 0;
    char _dirname[16] = {0};

    if (1 == sscanf(_str, "dir %s", _dirname))
    {
        fdir_t *_ndir = dir(_dirname, _ctx->_cdir);
        if (!_ndir)
            aoc_err("failed to create %s", _dirname);
        return 0;
    }
    return EINVAL;
}

int parse_all(aoc_context_h _ctx, char *_str)
{
    aoc_info("%s", _str);
    for (size_t _ii = 0; _ii < ARRAY_DIM(_ctx->_parsers); _ii++)
    {
        _ctx->_parsers[_ii]->_func(_ctx, _str);
    }
}