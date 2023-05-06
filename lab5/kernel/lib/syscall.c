#include "syscall.h"
#include "current.h"
#include "sched.h"
#include "stddef.h"
#include "uart.h"
#include "cpio.h"
#include "malloc.h"
#include "mbox.h"
#include "signal.h"
#include "string.h"

int getpid(trapframe_t *tpf)
{
    // get curr thead pi
    tpf->x0 = curr_thread->pid;
    return curr_thread->pid;
}

size_t uartread(trapframe_t *tpf, char buf[], size_t size)
{
    int i = 0;
    // async read
    for (int i = 0; i < size; i++)
        buf[i] = uart_async_getc();
    // size i
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
    // create new thread
    thread_t *newt = thread_create(curr_thread->data);

    // copy signal handler
    for (int i = 0; i <= SIGNAL_MAX; i++)
        newt->signal_handler[i] = curr_thread->signal_handler[i];

    // copy thread
    newt->datasize = curr_thread->datasize;
    int parent_pid = curr_thread->pid;
    thread_t *parent_thread_t = curr_thread;

    // copy user stack into new process
    memcpy(newt->user_sp, curr_thread->user_sp, USTACK_SIZE);

    // copy kernel stack into new process
    memcpy(newt->kernel_sp, curr_thread->kernel_sp, KSTACK_SIZE);

    store_context(current_ctx); // set child lr to here

    // for child
    if (parent_pid != curr_thread->pid)
        goto child;

    //newt->context = curr_thread->context; // buggy on rasbpi
    // move callee-saved registers
    newt->context.x19 = curr_thread->context.x19;
    newt->context.x20 = curr_thread->context.x20;
    newt->context.x21 = curr_thread->context.x21;
    newt->context.x22 = curr_thread->context.x22;
    newt->context.x23 = curr_thread->context.x23;
    newt->context.x24 = curr_thread->context.x24;
    newt->context.x25 = curr_thread->context.x25;
    newt->context.x26 = curr_thread->context.x26;
    newt->context.x27 = curr_thread->context.x28;
    newt->context.x28 = curr_thread->context.x28;
    newt->context.fp = curr_thread->context.fp + newt->kernel_sp - curr_thread->kernel_sp; // move fp
    newt->context.lr = curr_thread->context.lr;
    newt->context.sp = curr_thread->context.sp + newt->kernel_sp - curr_thread->kernel_sp; // move kernel sp

    unlock();

    tpf->x0 = newt->pid;
    return newt->pid;

child:
    tpf = (trapframe_t *)((char *)tpf + (unsigned long)newt->kernel_sp - (unsigned long)parent_thread_t->kernel_sp); // move tpf
    tpf->sp_el0 += newt->user_sp - parent_thread_t->user_sp;
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
    // ch & 0xf setting channel  &~0xF send setting of mbox
    unsigned long r = (((unsigned long)((unsigned long)mbox) & ~0xF) | (ch & 0xF));
    /* wait until we can write to the mailbox */
    do
    {
        asm volatile("nop");
    } while (*MBOX_STATUS & MBOX_FULL);
    // check status to decide
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