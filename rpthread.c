// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
tcb *head;
tcb *tail;
ucontext_t* curr_cctx;

// tcb *head , *tail; //  > might not be able to create global head and tail
// .*head = *tail = NULL;

boolean first_time_creating = TRUE; // variable used to check if pthread_create has ever been run before
ucontext_t * uctx_current , * uctx_sched; // current thread context; scheduler context


void context_test(int param){
    printf("%d",param);
}

int main()
{

    int i=0;
    for(i;i<3;i++) {
        void* param = i;
        pthread_t *thread = (pthread_t *) malloc(sizeof(pthread_t));
        rpthread_create(thread, NULL, &context_test, param);
    }
    //curr_cctx=head->t_context; idk if i need to do this
    setcontext(head->t_context);
    rpthread_yield();
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
	/* Since first time calling then we need to save "main" to tcb block*/
	// ! REMEMBER TO ADD CURRENT TO QUEUE FOR THE FIRST TIME CAUSE IT CONTAINS MAIN
	if (first_time_creating){
		first_time_creating = FALSE; // make sure never run again

		thread_queue = (ML_queue*) malloc(sizeof(ML_queue));
		thread_queue-> head = NULL;
		thread_queue-> tail = NULL;


		//since first time running, we can assume called from main
		create_tcb_main();
		//create scheduler context, but do not make it a tcb block
		create_scheduler_context();
		//create timer
		init_timer();

		// ! DO NOT ENQUEUE main b/c when call scheduler, you will enqueue current context there, which is main
	}


	//Establish the new thread
    void *thread_stack = malloc(STACK_SIZE);
	if (thread_stack == NULL ){
		perror("Failed to allocate stack: Pthread_create(thread_stack)");
		exit(1);
	}
    ucontext_t * uctx_new_thread= (ucontext_t*)malloc(sizeof(ucontext_t));
    uctx_new_thread->uc_link=NULL;//need to link this something? do not point this to the exit function because the exit function might run twice -DAVID
    uctx_new_thread->uc_stack.ss_sp=thread_stack;
    uctx_new_thread->uc_stack.ss_size=STACK_SIZE;
    uctx_new_thread->uc_stack.ss_flags=0;//dont know what this does

	// check if getcontext fails
    if (getcontext(uctx_new_thread) < 0){
        perror("getcontext:failure in pthread_create(uctx_new_thread)");
        exit(1);
    }

	// makeconext, based on args
	if(arg){ // arguments passed
		makecontext(uctx_new_thread,(void*)function,1,arg);
	}
	else{  // no args given
		makecontext(uctx_new_thread,(void*)function,0);
	}

	// ! ALOT MISSING FROM TCB BLOCK, REVISE LATER
	// TODO REVISE
	tcb * tcb_newthread = (tcb*)malloc(sizeof(tcb));
	tcb_newthread->tid = 0;
	tcb_newthread->thread_status = READY;
	tcb_newthread->priority = 0;
	tcb_newthread->t_context = uctx_new_thread;

	//! ENQUEUE THIS tcb_newthread

	/* switch to the scheduler */
	scheduler();


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
	getitimer(ITIMER_PROF, &timer);
	timer.it_value.tv_usec = 0; // stop the timer
	setitimer(ITIMER_PROF, &timer, NULL);
	swapcontext(uctx_current, uctx_sched ); // save current context, and then switch to scheduler

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
static void schedule() {
    // Every time when timer interrup happens, your thread library
    // should be contexted switched from thread context to this
    // schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

    while (1) {// is the while loop calling the same sub rountine schedule func over and over?

        if (sctf_flag) {
            sched_stcf();
        } else {
            sched_mlfq();
        }

    }

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
