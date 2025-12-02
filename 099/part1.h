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
#include <errno.h>

#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>

typedef enum
{
    QUEUE_MSG_INPUT = 0,
    QUEUE_MSG_KILL,
    QUEUE_MSG_KEYB,
    QUEUE_MSG_MAX
} evt_e;

#define MQ_MSG_MAX_SIZE (32)

typedef struct
{
    evt_e _eType;
    void *_vData;
} queue_msg_t;

typedef struct
{
    mqd_t _mqid;
    char *_mqname;
    struct mq_attr _attr;
    mode_t _xmode;
} queue_t;

typedef queue_t *queue_h;

typedef struct
{
    pthread_t _thr;
    struct timespec _timespec;
    aoc_2d_engine_h _eng_h;
    aoc_2d_object_h _cur_h;
    struct dll_head _objects_ll;
    int _iRet;
} gfx_task_t;

typedef struct
{
    pthread_t _thr;
    struct timespec _timespec;
    aoc_ring_buffer_h _pxInputCharBuffer;
    struct dll_head _inputs_ll;
    int _iRet;
} keyb_task_t;

struct context
{
    pthread_t _threads[2];
    gfx_task_t _gfxtask;
    keyb_task_t _inputstask;
    int _result;
};

#define CTX_CAST(_p) ((struct context *)_p)
#define GFX_CAST(_p) ((gfx_task_t *)_p)
#define KEYB_CAST(_p) ((keyb_task_t *)_p)
#define THR_CAST(_p) ((thead_data_t *)_p)

#define THREAD_OF(_tsk_) (_ctx->_tsk_._thr)

#define MAP_MID_SIZE (16)

#define MQ_NAME_SIZE_UINT (8)
#define MQ_NAME_SIZE_FMT "16"

static struct solutionCtrlBlock_t privPart1;
void *graphics_routine(void *args);
void *inputs_routine(void *args);
