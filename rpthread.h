// File:	rpthread_t.h

// List all group member's name:
// username of iLab:
// iLab Server:

#ifndef RTHREAD_T_H
#define RTHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_RTHREAD macro */
#define USE_RTHREAD 1
#define STACK_SIZE SIGSTKSZ


/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <ucontext.h>
#include <string.h>
#include <sys/time.h>

typedef enum
{
	/*A RUNNING thread is the thread that is currently executing on a processor. 
	The RUNNING thread is selected from the list of Ready threads for that processor, 
	which are threads that are good to be swapped in to execute
	*/
	READY, 
	SCHEDULED,
	RUNNING,
	BLOCKED,
	TERMINATED
} status;

typedef uint rpthread_t;

typedef struct threadControlBlock
{
	/* add important states in a thread control block */
	// thread Id
	rpthread_t tid;
	//status t_status;
	ucontext_t *t_context;
	status thread_status;
	int priority;
	//do i need a stack here or does context have it?
	struct threadControlBlock *next; //make a linkedlist or queue out of this

	// thread status
	// thread context
	// thread stack
	// thread priority

	// And more ...

	// YOUR CODE HERE
} tcb;

// typedef struct Queue
// {
// 	tcb *front;
// 	tcb *end;

// } queue;

/* mutex struct definition */
typedef struct rpthread_mutex_t
{
	/* add something here */

	// YOUR CODE HERE
} rpthread_mutex_t;

/* define your data structures here: */
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE

/* Function Declarations: */




/* create a new thread */
int rpthread_create(rpthread_t *thread, pthread_attr_t *attr, void *(*function)(void *), void *arg);

/* give CPU pocession to other user level threads voluntarily */
int rpthread_yield();

/* terminate a thread */
void rpthread_exit(void *value_ptr);

/* wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr);

/* initial the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, const pthread_mutexattr_t
													 *mutexattr);

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex);

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex);

/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex);

//rahil functions
tcb *tcb_init(ucontext_t* cctx,rpthread_t id);

/* scheduler */
static void schedule();

void enqueue(tcb *tcb_node);

void printQueue();

tcb *dequeue();

/* Create scheduler context*/
void create_scheduler_context();

#ifdef USE_RTHREAD
#define pthread_t rpthread_t
#define pthread_mutex_t rpthread_mutex_t
#define pthread_create rpthread_create
#define pthread_exit rpthread_exit
#define pthread_join rpthread_join
#define pthread_mutex_init rpthread_mutex_init
#define pthread_mutex_lock rpthread_mutex_lock
#define pthread_mutex_unlock rpthread_mutex_unlock
#define pthread_mutex_destroy rpthread_mutex_destroy
#endif

#endif
