#include "syscall.h"
#include "current.h"
#include "sched.h"
#include "stddef.h"
#include "uart.h"
#include "cpio.h"
#include "irq.h"
#include "malloc.h"
#include "mbox.h"
#include "signal.h"
#include "string.h"

int getpid(trapframe_t *tpf)
{
    tpf->x0 = curr_thread->pid;
    return curr_thread->pid;
}

size_t uartread(trapframe_t *tpf, char buf[], size_t size)
{
    int i = 0;
    for (int i = 0; i < size; i++)
        buf[i] = uart_async_getc();

    tpf->x0 = i;
    return i;
}

size_t uartwrite(trapframe_t *tpf, const char buf[], size_t size)
{
    int i = 0;
    for (int i = 0; i < size; i++)
        uart_async_putc(buf[i]);
    
    tpf->x0 = i;
    return i;
}

// In this lab, you won’t have to deal with argument passing
int exec(trapframe_t *tpf, const char *name, char *const argv[])
{
    curr_thread->datasize = get_file_size((char *)name);
    char *new_data = get_file_start((char *)name);
    memcpy(curr_thread->data, new_data, curr_thread->datasize); 

    // clear signal handler
    for (int i = 0; i <= SIGNAL_MAX; i++)
        curr_thread->signal_handler[i] = signal_default_handler;

    tpf->elr_el1 = (unsigned long)curr_thread->data;
    tpf->sp_el0 = (unsigned long)curr_thread->user_sp + USTACK_SIZE;
    tpf->x0 = 0;
    return 0;
}

int fork(trapframe_t *tpf)
{
    lock();
    //uart_async_printf("t1\n");
    thread_t *child_thread = thread_create(curr_thread->data);
    //uart_async_printf("t2\n");
    // copy signal handler
    for (int i = 0; i <= SIGNAL_MAX; i++)
        child_thread->signal_handler[i] = curr_thread->signal_handler[i];
    //uart_async_printf("t3\n");
    int parent_pid = curr_thread->pid;
    thread_t *parent_thread = curr_thread;

    // copy data
    //child_thread->data = malloc(curr_thread->datasize);
    //child_thread->datasize = curr_thread->datasize;
    //memcpy(child_thread->data, curr_thread->data, curr_thread->datasize);

    // copy user stack into new process
    memcpy(child_thread->user_sp, curr_thread->user_sp, USTACK_SIZE);

    // copy kernel stack into new process
    memcpy(child_thread->kernel_sp, curr_thread->kernel_sp, KSTACK_SIZE);

    store_context(current_ctx); // set child lr to here
    // for child
    if (parent_pid != curr_thread->pid)
        goto child;

    child_thread->context.x19 = curr_thread->context.x19;
    child_thread->context.x20 = curr_thread->context.x20;
    child_thread->context.x21 = curr_thread->context.x21;
    child_thread->context.x22 = curr_thread->context.x22;
    child_thread->context.x23 = curr_thread->context.x23;
    child_thread->context.x24 = curr_thread->context.x24;
    child_thread->context.x25 = curr_thread->context.x25;
    child_thread->context.x26 = curr_thread->context.x26;
    child_thread->context.x27 = curr_thread->context.x28;
    child_thread->context.x28 = curr_thread->context.x28;
    child_thread->context.fp =  child_thread->kernel_sp + curr_thread->context.fp - curr_thread->kernel_sp; // move fp
    child_thread->context.lr = curr_thread->context.lr;
    child_thread->context.sp =  child_thread->kernel_sp + curr_thread->context.sp - curr_thread->kernel_sp; // move kernel sp

    unlock();

    tpf->x0 = child_thread->pid;
    return child_thread->pid;

child:
    tpf = (trapframe_t *)((unsigned long)child_thread->kernel_sp + (char *)tpf - (unsigned long)parent_thread->kernel_sp); // move tpf
    tpf->sp_el0 = child_thread->user_sp + tpf->sp_el0 - parent_thread->user_sp;
    //tpf->elr_el1 = (unsigned long)child_thread->data;
    tpf->x0 = 0;
    return 0;
}

void exit(trapframe_t *tpf, int status)
{
    thread_exit();
}

int syscall_mbox_call(trapframe_t *tpf, unsigned char ch, unsigned int *mbox)
{
    lock();
    unsigned long r = (((unsigned long)((unsigned long)mbox) & ~0xF) | (ch & 0xF));
    /* wait until we can write to the mailbox */
    do
    {
        asm volatile("nop");
    } while (*MBOX_STATUS & MBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    *MBOX_WRITE = r;
    /* now wait for the response */
    while (1)
    {
        /* is there a response? */
        do
        {
            asm volatile("nop");
        } while (*MBOX_STATUS & MBOX_EMPTY);
        /* is it a response to our message? */
        if (r == *MBOX_READ)
        {
            /* is it a valid successful response? */
            tpf->x0 = (mbox[1] == MBOX_RESPONSE);
            unlock();
            return mbox[1] == MBOX_RESPONSE;
        }
    }

    tpf->x0 = 0;
    unlock();
    return 0;
}

void kill(trapframe_t *tpf, int pid)
{
    lock();
    if (pid >= PIDMAX || pid < 0 || threads[pid].status == FREE)
    {
        unlock();
        return;
    }
    threads[pid].status = DEAD;
    unlock();
    schedule();
}

void signal_register(int signal, void (*handler)())
{
    if (signal > SIGNAL_MAX || signal < 0)
        return;

    curr_thread->signal_handler[signal] = handler;
}

void signal_kill(int pid, int signal)
{
    if (pid >= PIDMAX || pid < 0 || threads[pid].status == FREE)
        return;

    lock();
    threads[pid].sigcount[signal]++;
    unlock();
}

void sigreturn(trapframe_t *tpf)
{
    unsigned long signal_ustack = tpf->sp_el0 % USTACK_SIZE == 0 ? tpf->sp_el0 - USTACK_SIZE : tpf->sp_el0 & (~(USTACK_SIZE - 1));
    free((char *)signal_ustack);
    load_context(&curr_thread->signal_saved_context);
}
