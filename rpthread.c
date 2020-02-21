// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:


#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
tcb *head;
tcb *tail;


// queue *queue_init()
// {
// 	queue *tcb_queue = (queue *)malloc(sizeof(queue));
// 	return tcb_queue;
// }

int main()
{

	tcb *n1 = tcb_init(1);
	enqueue(n1);
	tcb *n2 = tcb_init(2);
	enqueue(n2);
	//dequeue();
	tcb *n3 = tcb_init(3);
	enqueue(n3);
	printQueue();
	return 0;
}

tcb* tcb_init(int priority)
{
	tcb *newNode = (tcb *)malloc(sizeof(tcb));
	newNode->tid = NULL;
	//status
	newNode->t_context = NULL;
	newNode->priority = priority;
	newNode->stack = NULL;
	newNode->next = NULL;
	return newNode;
}
// YOUR CODE HERE

void enqueue(tcb *tcb_node)
{
	if (head == NULL)
	{
		head = tcb_node;
		tail = tcb_node;
	}
	else
	{
		tail->next = tcb_node;
		tail = tcb_node;
	}
}

int getQueueSize()
{
	tcb *curr = head;
	int size = 0;
	while (curr != NULL)
	{
		size++;
		curr = curr->next;
	}
	return size;
}

void printQueue()
{
	tcb *curr = head;

	while (curr != NULL)
	{
		printf("%d\t", curr->priority);
		curr = curr->next;
	}
}

tcb *dequeue()
{
	if (getQueueSize() != 0)
	{
		tcb *first = head;
		head = head->next;
		return first;
	}
	else
	{
		puts("cant dequeue from empty Q");
		return NULL;
	}
}

/* create a new thread */
int rpthread_create(rpthread_t *thread, pthread_attr_t *attr,
					void *(*function)(void *), void *arg)
{
	// Create Thread Control Block
	// Create and initialize the context of this thread
	// Allocate space of stack for this thread to run
	// after everything is all set, push this thread int
	// YOUR CODE HERE

	return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield()
{

	// Change thread state from Running to Ready
	// Save context of this thread to its thread control block
	// switch from thread context to scheduler context

	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr){
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
};

/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr)
{

	// Wait for a specific thread to terminate
	// De-allocate any dynamic memory created by the joining thread

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex,
						const pthread_mutexattr_t *mutexattr)
{
	//Initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex)
{
	// use the built-in test-and-set atomic function to test the mutex
	// When the mutex is acquired successfully, enter the critical section
	// If acquiring mutex fails, push current thread into block list and
	// context switch to the scheduler thread

	// YOUR CODE HERE
	return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex)
{
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};

/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex)
{
	// Deallocate dynamic memory created in rpthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule()
{
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// schedule policy
#ifndef MLFQ
	// Choose STCF
#else
	// Choose MLFQ
#endif
}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf()
{
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq()
{
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

// Feel free to add any other functions you need

// YOUR CODE HERE
