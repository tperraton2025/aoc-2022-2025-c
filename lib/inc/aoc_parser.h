#ifndef AOC_PARSER_H
#define AOC_PARSER_H
#include <errno.h>
#include "aoc_types.h"
#include "aoc_ranges.h"
#include "aoc_linked_list.h"

/**
 * Text input parsers
 */

typedef struct parser
{
    struct dll_node _node;
    char *_name;
    void *arg;
    size_t _parsed;
    int (*_func)(void *arg, char *_str);
} parser_t;
typedef parser_t *parser_h;

parser_h parser_list_init();
int parser_append(dll_head_h head, parser_h const parser, void *arg);
int parse_all(dll_head_h head, char *str);
void parser_list_free(parser_h *list);
#endif