#ifndef PART_X_H
#define PART_X_H
#include <string.h>
#include <stdlib.h>
#include <aoc_linked_list.h>
#include <aoc_parser.h>
#include <aoc_tree.h>

#define NAME_MAX_LEN (16)

typedef struct
{
    struct tree_node path;
    struct dll_head _content;
    char *name;
    size_t size;
} fdir_t;

typedef struct
{
    fdir_t *dir;
    size_t size;
    char *name;
} file_t;

typedef struct
{
    struct dll_node _llnode;
    fdir_t *_fref;
} path_step_t;

extern int command_parser(aoc_context_h _ctx, char *_str);
extern int file_parser(aoc_context_h _ctx, char *_str);
extern int dir_parser(aoc_context_h _ctx, char *_str);
int parse_all(aoc_context_h _ctx, char *_str);

static struct parser _cmd_parser = {._name = "cmd parser", ._parseRegx = "$ %s %s", ._func = command_parser};
static struct parser _file_parser = {._name = "file parser", ._parseRegx = "%ld %s.%s", ._func = file_parser};
static struct parser _dir_parser = {._name = "dir parser", ._parseRegx = "%%s %s", ._func = dir_parser};

static bool find_dir_by_name(void *_a, void *_b)
{
    if (!_a || !_b)
    {
        aoc_err("abnormal pointers provided in %s(%p,%p)", __func__, _a, _b);
        return false;
    }
    fdir_t *_pdir = (fdir_t *)_a;
    return (0 == strncmp(_pdir->name, (char *)_b, NAME_MAX_LEN) ? true : false);
}

static void pwd(aoc_tree_node_h _start)
{
    fdir_t *_fsp = (fdir_t *)_start;
    struct dll_head path = {0};
    while (_fsp)
    {
        path_step_t *_step = malloc(sizeof(path_step_t));
        _step->_fref = _fsp;
        dll_node_append(&path, &_step->_llnode);
        _fsp = (fdir_t *)_fsp->path._parent;
    }
    printf("\n");
    LL_FOREACH_LAST(_llnode, path)
    {
        printf("%s/", ((fdir_t *)_llnode)->name);
    }
    printf("\n");
    dll_free_all(&path, free);
}

void delete_dir(void *arg)
{
    fdir_t *_dir = (fdir_t *)arg;
    free(_dir->name);
    aoc_tree_free_all_children(&_dir->path);
}

fdir_t *dir(const char *const name, fdir_t *parent)
{
    fdir_t *_ndir = malloc(sizeof(fdir_t));
    if (!_ndir)
        return NULL;
    _ndir->name = malloc(strnlen(name, NAME_MAX_LEN) + 1);
    if (!_ndir->name)
    {
        free(_ndir);
        return NULL;
    }
    strcpy(_ndir->name, name);
    aoc_tree_node(&_ndir->path, parent ? &parent->path : NULL, delete_dir);
    _ndir->path._parent = parent ? &parent->path : NULL;
}

file_t *file(char *name, size_t size, fdir_t *dir)
{
    file_t *_nfile = malloc(sizeof(file_t));
    _nfile->name = malloc(strnlen(name, NAME_MAX_LEN));
    _nfile->name = name;
    _nfile->size = size;
    _nfile->dir = dir;
}

void delete_file(file_t *_file)
{
    free(_file->name);
    free(_file);
}

#endif
