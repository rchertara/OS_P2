// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
tcb *head , tail;
head = tail = NULL;

boolean first_time_creating = TRUE; 
ucontext_t* curr_cctx , uctx_sched;


// queue *queue_init()
// {
// 	queue *tcb_queue = (queue *)malloc(sizeof(queue));
// 	return tcb_queue;
// }

void create_scheduler_context(){

	/*create stack and check if memory allocation good*/
	void *stack_sched = malloc(STACK_SIZE);
	if (stack_sched== NULL)	{
		perror("Failed to allocate stack");
		exit(1);
	}
	// use getcontext to avoid segfault
	if (getcontext(&uctx_sched) < 0){
		perror("getcontext");
		exit(1);
	}

	// set the necessary values 
	uctx_sched.uc_stack.ss_sp = stack_sched;
	uctx_sched.uc_stack.ss_size = STACK_SIZE;
	uctx_sched.uc_stack.ss_flags = 0; 
	uctx_sched.uc_link  = NULL;

	// make the context for future use 
	makecontext(&uctx_sched, (void *)&schedule,0);
}




void context_test(int param){
    printf("%d",param);
}

int main()
{

	return 0;
}





tcb *tcb_init(ucontext_t* cctx,rpthread_t id)
{
	tcb *newNode = (tcb *)malloc(sizeof(tcb));
	newNode->tid = id;
	//NEED TO SET STATUS
	newNode->t_context =cctx;
    newNode->priority = 0;//default val
	//need stack?
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
{//not super good method since has to pass whole time to check size
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
		printf("%d\t", curr->tid);
		curr = curr->next;
	}
    printf("\n");//new line
}
tcb* find_tid(rpthread_t goal) // type 'uint'
{
	tcb *curr = head;
	if(head == NULL){
		return NULL;
	}
	
	while (curr != NULL)
	{
		if(curr->tid == goal )
		{
			return curr;
		}
		curr = curr->next;
	}

	// did NOT find node, but still return 
	return NULL;
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
/*
if queue empty then do the following =>
create a context for main(TCB) and schedule (global context)
call schedule inside of here 

-------------------------------------
normal functions
create desired thread

//start the timer, 
//signal handler is yield
//  if someting is blocked, avoid using it, then enqueue it till have a ready thread 
//don't set the context return function to pthread exit

*/
int rpthread_create(rpthread_t *thread, pthread_attr_t *attr,
					void *(*function)(void *), void *arg)
{
	if (first_time_creating)
	{
		first_time_creating = FALSE; // make sure never run again
		if (getcontext(&uctx_main) < 0){// save context of main
		perror("getcontext");
		exit(1);
	}
		// allocate memory for the main stack just in case

		void *stack_main  = malloc (STACK_SIZE);
		if(stack_main== NULL){
			perror("Could not allocate stack for main");
			exit(1);
		}
		uctx_main.uc_stack.ss_sp = stack_main;
		uctx_main.uc_stack.ss_size = STACK_SIZE;
		uctx_main.uc_stack.ss_flags = 0; 
		uctx_main.uc_link  = NULL;  // this points to the function it
		makecontext(&)
		// ! makecontext (&uctx_main, linktoexit )
		// TODO SHD
	}
	
    void *thread_stack = malloc(STACK_SIZE);
    ucontext_t * cctx= (ucontext_t*)malloc(sizeof(ucontext_t));
    cctx->uc_link=NULL;//need to link this something?
    cctx->uc_stack.ss_sp=thread_stack;
    cctx->uc_stack.ss_size=STACK_SIZE;
    cctx->uc_stack.ss_flags=0;//dont know what this does

	// Create Thread Control Block


    if (getcontext(cctx) < 0){
        perror("getcontext");
        exit(1);
    }
    //do i call get context before make context?
    makecontext(cctx ,(void *) function,1,arg);
    tcb * tcbNode= tcb_init(cctx,*thread);
    enqueue(tcbNode);
	// Create and initialize the context of this thread
	// Allocate space of stack for this thread to run (is this the cctx field or something else?)
	// after everything is all set, push this thread int (push on queue what?)
	// YOUR CODE HERE

	return 0;
};

/* give CPU possession to other user-level threads voluntarily */
// timer reset and signal handler function
int rpthread_yield()
{

	// Change thread state from Running to Ready
	// Save context of this thread to its thread control block
	// switch from thread context to scheduler context
	/* 
	1. Update some struct value (READY)
	2. Run getcontext on current context
	3. stop timer
	4. switch to scheduler function (i.e. the thread that handles scheduling)
	*/

	//MAKE SURE NOT WORKING WITH EMPTY LL
	struct itimerval timer;
	getitimer(ITIMER_PROF, &timer, NULL)
	timer.it_value.tv

	return 0;
};

/* terminate a thread */
//
void rpthread_exit(void *value_ptr){
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
};

/* Wait for thread termination */
// inside of tcb keep a list of all threads waiting to join  
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
// no need to add paramters; just use global variables 
//remove terminated item from tcb
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

// * UNDELETE THIS STUFF LATER ! 
// /*  Preemptive SJF (STCF) scheduling algorithm */
// static void sched_stcf()
// {
// 	// Your own implementation of STCF
// 	// (feel free to modify arguments and return types)

// 	// YOUR CODE HERE
// 	return;  // ! delete later
// }


// /* Preemptive MLFQ scheduling algorithm */
// static void sched_mlfq()
// {
// 	// Your own implementation of MLFQ
// 	// (feel free to modify arguments and return types)

// 	// YOUR CODE HERE
// 	return;  // ! delete later
// }




// Feel free to add any other functions you need

// YOUR CODE HERE
