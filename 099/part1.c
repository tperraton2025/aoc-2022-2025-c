#include "part1.h"
#include <termios.h>

static pthread_t _threads[2] = {0};
static sem_t _tasks_start_sem;
static sem_t _tasks_kill_sem;
static queue_t _evtqueue;

#define EVT_QUEUE_LEN (256)
#define EVT_SIZE_IN_BYTES (1)

static struct solutionCtrlBlock_t privPart1;
void *graphics_routine(void *args);
void *inputs_routine(void *args);

void sig_int_handler(int args)
{
    struct context *_ctx = CTX_CAST(privPart1._data);
    aoc_warn("received signal %s", strsignal(args));

    if (SIGINT & args)
    {
        for (size_t _itask = 0; _itask < 2; _itask++)
        {
            sem_post(&_tasks_kill_sem);
        }
    }
}

static void queue_setup(queue_h mq_h, const char *_name, size_t mqlen, size_t msgsize)
{
    size_t _namelen = strnlen(_name, MQ_NAME_SIZE_UINT);
    mq_h->_mqname = malloc(_namelen + 1);
    snprintf(mq_h->_mqname, MQ_NAME_SIZE_UINT, "%s", _name);
    mq_h->_xmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    mq_h->_attr.mq_curmsgs = 0;
    mq_h->_attr.mq_flags = 0;
    mq_h->_attr.mq_maxmsg = mqlen;
    mq_h->_attr.mq_msgsize = msgsize;

    mq_h->_mqid = mq_open(mq_h->_mqname, O_RDWR | O_NONBLOCK | O_CREAT, mq_h->_xmode, &mq_h->_attr);
}

static int prologue(struct solutionCtrlBlock_t *_blk, int argc, char *argv[])
{
    int _ret = 1;
    _blk->_data = malloc(sizeof(struct context));
    if (!_blk->_data)
        return ENOMEM;
    struct context *_ctx = CTX_CAST(_blk->_data);
    memset(_ctx, 0, sizeof(struct context));

    _ctx->_result = 0; 

    _ret = sem_init(&_tasks_start_sem, 0, 0);
    if (_ret)
        goto error;

    _ret = sem_init(&_tasks_kill_sem, 0, 0);
    if (_ret)
        goto error;

    signal(SIGINT, sig_int_handler);
    queue_setup(&_evtqueue, "/inmq", EVT_QUEUE_LEN, EVT_SIZE_IN_BYTES);

    for (size_t i = 0; i < 1; i++)
    {
        /* code */
    }
    
    pthread_create(&THREAD_OF(_gfxtask), NULL, graphics_routine, &_ctx->_gfxtask);
    pthread_create(&THREAD_OF(_inputstask), NULL, inputs_routine, &_ctx->_inputstask);

    for (size_t _itask = 0; _itask < ARRAY_DIM(_threads); _itask++)
    {
        sem_post(&_tasks_start_sem);
    }

    pthread_join(_ctx->_gfxtask._thr, NULL);
    pthread_join(_ctx->_inputstask._thr, NULL);

    sem_destroy(&_tasks_start_sem);
    sem_destroy(&_tasks_kill_sem);
    free(_evtqueue._mqname);
    return 0;

error:
    free(_blk->_data);
    return _ret;
}

static int handler(struct solutionCtrlBlock_t *_blk)
{
    struct context *_ctx = CTX_CAST(_blk->_data);
    return 0;
}

static int epilogue(struct solutionCtrlBlock_t *_blk)
{
    aoc_ans("AOC 2022 %s solution is %d", _blk->_name, 0);
    free(_blk->_data);
    return 0;
}

void *graphics_routine(void *args)
{
    gfx_task_t *_gfxtask = GFX_CAST(args);

    coord_t resolution = {._x = 1, ._y = 1};
    coord_t spaceing = {._x = 0, ._y = 0};

    ALLOCATE_AND_RETURN_IF_NULL(_gfxtask->_eng_h, engine_create(resolution, spaceing, '.', 0),
                                _gfxtask->_iRet,
                                ENOMEM,
                                exit);

    dll_head_init(&_gfxtask->_objects_ll);

    aoc_engine_resize_one_direction(_gfxtask->_eng_h, MAP_MID_SIZE, AOC_DIR_RIGHT);
    aoc_engine_resize_one_direction(_gfxtask->_eng_h, MAP_MID_SIZE, AOC_DIR_LEFT);
    aoc_engine_resize_one_direction(_gfxtask->_eng_h, MAP_MID_SIZE >> 1, AOC_DIR_UP);
    aoc_engine_resize_one_direction(_gfxtask->_eng_h, MAP_MID_SIZE >> 1, AOC_DIR_DOWN);

    ALLOCATE_AND_RETURN_IF_NULL(_gfxtask->_cur_h,
                                eng_obj_create(_gfxtask->_eng_h, "cursor", NULL, "H", OBJ_PROPERTY_NO_COLLISION | OBJ_PROPERTY_MOBILE),
                                _gfxtask->_iRet,
                                ENOMEM,
                                exit);

    ASSIGN_AND_RETURN_IF_NOT_0(_gfxtask->_iRet,
                               aoc_engine_append_obj(_gfxtask->_eng_h, _gfxtask->_cur_h),
                               free_object);

    engine_draw(_gfxtask->_eng_h);

    char _cMsgQueueBuffer[sizeof(queue_msg_t) + 1] = {0};

    int keystrokeprio = 1;
    int _ret = sem_wait(&_tasks_start_sem);

    int mq_ret = mq_receive(_evtqueue._mqid, _cMsgQueueBuffer, 1, &keystrokeprio);


    while (sem_trywait(&_tasks_kill_sem))
    {
        mq_ret = mq_receive(_evtqueue._mqid, _cMsgQueueBuffer, 1, NULL);
        if (mq_ret > 0)
        {
            if (AOC_DIR_UP == _cMsgQueueBuffer[0])
            {
                sprintf(_cMsgQueueBuffer, "UP");
                aoc_engine_move_object_and_redraw(_gfxtask->_eng_h, _gfxtask->_cur_h, 1, AOC_DIR_UP);
            }
            else if (AOC_DIR_DOWN == _cMsgQueueBuffer[0])
            {
                sprintf(_cMsgQueueBuffer, "DOWN");
                aoc_engine_move_object_and_redraw(_gfxtask->_eng_h, _gfxtask->_cur_h, 1, AOC_DIR_DOWN);
            }
            else if (AOC_DIR_RIGHT == _cMsgQueueBuffer[0])
            {
                sprintf(_cMsgQueueBuffer, "RIGHT");
                aoc_engine_move_object_and_redraw(_gfxtask->_eng_h, _gfxtask->_cur_h, 1, AOC_DIR_LEFT);
            }
            else if (AOC_DIR_LEFT == _cMsgQueueBuffer[0])
            {
                sprintf(_cMsgQueueBuffer, "LEFT");
                aoc_engine_move_object_and_redraw(_gfxtask->_eng_h, _gfxtask->_cur_h, 1, AOC_DIR_RIGHT);
            }
            aoc_engine_prompt(_gfxtask->_eng_h, 0, 2, "got mq", _cMsgQueueBuffer);
            printf("\n");
            mq_ret = 0;
        }
        usleep(5 * 1000);
    }

    goto exit;

free_object:
    if (_gfxtask->_cur_h)
        eng_obj_free(_gfxtask->_cur_h);
exit:
    mq_unlink(_evtqueue._mqname);
    if (_gfxtask->_eng_h)
        engine_free(_gfxtask->_eng_h);

    mq_close(_evtqueue._mqid);
    aoc_info("%s exited with code: %i (%s)" RESET, __func__, _ret, strerror(_ret));
    pthread_exit(NULL);
}

void set_termios()
{
    struct termios term;
    tcgetattr(fileno(stdin), &term);
    term.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(fileno(stdin), 0, &term);
}

void restore_termios()
{
    struct termios term;
    tcgetattr(fileno(stdin), &term);
    term.c_lflag |= (ECHO | ICANON);
    tcsetattr(fileno(stdin), 0, &term);
}

void *inputs_routine(void *args)
{
    set_termios();
    atexit(restore_termios);

    keyb_task_t *_inp = KEYB_CAST(args);
    int _ret = fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    if (_ret)
        goto error;

    char _cByte = ' ';
    _inp->_pxInputCharBuffer = aoc_ring_buffer(sizeof(char), 64);
    _ret = sem_wait(&_tasks_start_sem);

    char _nchar[5] = {0};
    while (sem_trywait(&_tasks_kill_sem))
    {
        int _char_cnt = 0;
        _char_cnt = read(0, _nchar, 3);
        if (_char_cnt == 3)
        {
            if (0 == strncmp("\e[A", _nchar, 3))
            {
                char _msg = AOC_DIR_UP;
                mq_send(_evtqueue._mqid, &_msg, 1, 0);
            }
            else if (0 == strncmp("\e[B", _nchar, 3))
            {
                char _msg = AOC_DIR_DOWN;
                mq_send(_evtqueue._mqid, &_msg, 1, 0);
            }
            else if (0 == strncmp("\e[C", _nchar, 3))
            {
                char _msg = AOC_DIR_LEFT;
                mq_send(_evtqueue._mqid, &_msg, 1, 0);
            }
            else if (0 == strncmp("\e[D", _nchar, 3))
            {
                char _msg = AOC_DIR_RIGHT;
                mq_send(_evtqueue._mqid, &_msg, 1, 0);
            }
        }
        usleep(100 * 1000);
    }

error:
    if (0 < _evtqueue._mqid)
        mq_unlink(_evtqueue._mqname);
    if (_inp->_pxInputCharBuffer)
        aoc_ring_buffer_free(_inp->_pxInputCharBuffer);

    mq_close(_evtqueue._mqid);
    aoc_info("%s exited with code: %i (%s)" RESET, __func__, _ret, strerror(_ret));
    pthread_exit(NULL);
}

static void free_solution(struct solutionCtrlBlock_t *_blk)
{
}

static struct solutionCtrlBlock_t privPart1 = {._name = CONFIG_DAY " part 1", ._prologue = prologue, ._handler = handler, ._epilogue = epilogue, ._free = free_solution};
struct solutionCtrlBlock_t *part1 = &privPart1;
