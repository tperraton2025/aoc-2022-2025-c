#include <aoc_helpers.h>
#include <aoc_2d_engine.h>
#include <aoc_input.h>
#include <aoc_iterator.h>
#include <aoc_threads_helpers.h>
#include <aoc_ring_buffer.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>

typedef enum
{
    QUEUE_MSG_INPUT = 0,
    QUEUE_MSG_KILL,
    QUEUE_MSG_MAX
} queue_msg_type_enum_t;

#define MQ_MSG_MAX_SIZE (32)

typedef struct
{
    queue_msg_type_enum_t _eType;
    void *_vData;
} queue_msg_t;

typedef struct
{
    int _ret;
    void *_arg;
} thead_data_t;

typedef struct
{
    mqd_t _iMqueue;
    char *_sName;
    struct mq_attr _attr;
    mode_t _xMode;
} _hQueue_t;

typedef struct
{
    pthread_t _thr;
    _hQueue_t _xQueue;
    aoc_2d_engine_h _eng;
    aoc_2d_object_h _cur;
    struct dll_head _objects_ll;
    int _iRet;
} graphics_t;

typedef struct
{
    pthread_t _thr;
    _hQueue_t _xQueue;
    aoc_ring_buffer_h _pxInputCharBuffer;
    struct dll_head _inputs_ll;
    int _iRet;
} inputs_t;

struct context
{
    graphics_t _graphics;
    inputs_t _inputs;
    _hQueue_t *_hMqueues[8];
    sem_t *_tasks_sem;
    size_t _hMq_top;
    int result;
};

#define CTX_CAST(_p) ((struct context *)_p)
#define GRP_CAST(_p) ((graphics_t *)_p)
#define INP_CAST(_p) ((inputs_t *)_p)
#define THR_CAST(_p) ((thead_data_t *)_p)

#define MOV_CAST(_p) ((movement_t *)_p)
#define MAP_MID_SIZE (16)

#define MQ_NAME_SIZE_UINT (8)
#define MQ_NAME_SIZE_FMT "16"

static struct solutionCtrlBlock_t privPart1;
void *graphics_routine(void *args);
void *inputs_routine(void *args);
