
#include "sched.h"
#include "current.h"
#include "signal.h"
#include "uart.h"
#include "malloc.h"
#include "timer.h"
#include "string.h"


thread_t *curr_thread;
list_head_t *run_queue;
list_head_t *wait_queue;
thread_t threads[PIDMAX + 1];


void init_thread_sched()
{
    lock();
    run_queue = malloc(sizeof(list_head_t));
    wait_queue = malloc(sizeof(list_head_t));
    INIT_LIST_HEAD(run_queue);
    INIT_LIST_HEAD(wait_queue);

    // init pids
    for (int i = 0; i <= PIDMAX; i++)
    {
        threads[i].pid = i;
        threads[i].status = FREE;
    }

    thread_t *tmp = malloc(sizeof(thread_t));
    // set tpidr_el1
    set_current_ctx(&tmp->context);
    curr_thread = tmp;

    thread_create(idle);

    add_timer(schedule_timer, "", 1, 0);
    unlock();
}
void idle()
{
    while (1)
    {
        kill_zombies(); // reclaim threads marked as DEAD
        schedule();     // switch to next thread in run queue
    }
}
void schedule()
{
    lock();
    do
    {
        curr_thread = (thread_t *)curr_thread->listhead.next;
    } while (list_is_head(&curr_thread->listhead, run_queue) || curr_thread->status == DEAD);

    switch_to(current_ctx, &curr_thread->context);
    unlock();
}

void kill_zombies()
{
    lock();
    list_head_t *tmp;
    list_for_each(tmp, run_queue)
    {
        thread_t *tmp_thread = (thread_t *)tmp;
        if (tmp_thread->status == DEAD)
        {
            list_del(tmp);
            free(tmp_thread->user_sp);   // free stack
            free(tmp_thread->kernel_sp); // free stack
        //    free(tmp_thread->data); // free data
            tmp_thread->status = FREE;
        }
    }
    unlock();
}

int exec_thread(char *data, unsigned int filesize)
{
    thread_t *t = thread_create(data);
    t->data = malloc(filesize);
    t->datasize = filesize;
    t->context.lr = (unsigned long)t->data;
    // copy file into data
    memcpy(t->data, data, filesize);

    // disable echo when going to userspace
    echo = 0;
    curr_thread = t;
    // eret to exception level 0
    __asm__ __volatile__("msr tpidr_el1, %0\n\t"
                         "msr elr_el1, %1\n\t"
                         "msr spsr_el1, xzr\n\t"
                         "msr sp_el0, %2\n\t"
                         "mov sp, %3\n\t"
                         "eret\n\t" ::"r"(&t->context),
                         "r"(t->data), "r"(t->user_sp + USTACK_SIZE), "r"(t->kernel_sp + KSTACK_SIZE));

    return 0;
}

thread_t *thread_create(void *start)
{
    lock();

    thread_t *r;
    for (int i = 0; i <= PIDMAX; i++)
    {
        if (threads[i].status == FREE)
        {
            r = &threads[i];
            break;
        }
    }
    r->status = RUNNING;
    r->context.lr = (unsigned long long)start;
    r->user_sp = malloc(USTACK_SIZE);
    r->kernel_sp = malloc(KSTACK_SIZE);
    r->context.sp = (unsigned long long)r->kernel_sp + KSTACK_SIZE;
    r->context.fp = r->context.sp;
    r->signal_is_checking = 0;
    // initial signal handler with signal_default_handler (kill thread)
    for (int i = 0; i < SIGNAL_MAX; i++)
    {
        r->signal_handler[i] = signal_default_handler;
        r->sigcount[i] = 0;
    }

    list_add(&r->listhead, run_queue);
    unlock();
    return r;
}

void thread_exit()
{
    lock();
    curr_thread->status = DEAD;
    unlock();
    schedule();
}

void schedule_timer(char *notuse)
{
    unsigned long long cntfrq_el0;
    __asm__ __volatile__("mrs %0, cntfrq_el0\n\t"
                         : "=r"(cntfrq_el0)); // tick frequency
    add_timer(schedule_timer, "", cntfrq_el0 >> 5, 1);
}