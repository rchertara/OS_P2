// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "rpthread.h"

/* INITAILIZE ALL YOUR VARIABLES HERE*/

// tcb *head , *tail; //  > might not be able to create global head and tail
// .*head = *tail = NULL;

boolean first_time_creating = TRUE;    // variable used to check if pthread_create has ever been run before
ucontext_t *uctx_current, *uctx_sched; // current thread context; scheduler context

mlq *ml_queue[4]; // queue of all threads
struct itimerval mytime;

/* END OF GLOBAL VARIABLE INIT*/

int main()
{

    return 0;
}

/* create a new thread */
/*
if queue empty then do the following =>
create a context for main(TCB) and schedule (global context)
call schedule inside of here 
create timer here aswell
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
    /* Since first time calling then we need to save "main" to tcb block*/
    // ! REMEMBER TO ADD CURRENT TO QUEUE FOR THE FIRST TIME CAUSE IT CONTAINS MAIN
    if (first_time_creating)
    {
        first_time_creating = FALSE; // make sure never run again

        ml_queue_init();

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
    if (thread_stack == NULL)
    {
        perror("Failed to allocate stack: Pthread_create(thread_stack)");
        exit(1);
    }
    ucontext_t *uctx_new_thread = (ucontext_t *)malloc(sizeof(ucontext_t));
    uctx_new_thread->uc_link = NULL; //need to link this something? do not point this to the exit function because the exit function might run twice -DAVID
    uctx_new_thread->uc_stack.ss_sp = thread_stack;
    uctx_new_thread->uc_stack.ss_size = STACK_SIZE;
    uctx_new_thread->uc_stack.ss_flags = 0; //dont know what this does

    // check if getcontext fails
    if (getcontext(uctx_new_thread) < 0)
    {
        perror("getcontext:failure in pthread_create(uctx_new_thread)");
        exit(1);
    }

    // makeconext, based on args
    if (arg)
    { // arguments passed
        makecontext(uctx_new_thread, (void *)function, 1, arg);
    }
    else
    { // no args given
        makecontext(uctx_new_thread, (void *)function, 0);
    }

    // ! ALOT MISSING FROM TCB BLOCK, REVISE LATER
    // TODO REVISE
    tcb *tcb_newthread = (tcb *)malloc(sizeof(tcb));
    tcb_newthread->tid = 0;
    tcb_newthread->t_status = READY;
    tcb_newthread->priority = 0;
    tcb_newthread->t_context = uctx_new_thread;

    //! ENQUEUE THIS tcb_newthread
    enqueue(tcb_newthread);

    /* switch to the scheduler */
    schedule(); // idk if this is right

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
    //update TCB here
    //MAKE SURE NOT WORKING WITH EMPTY LL
    getitimer(ITIMER_PROF, &mytime);
    mytime.it_value.tv_usec = 0; // stop the timer
    setitimer(ITIMER_PROF, &mytime, NULL);
    swapcontext(uctx_current, uctx_sched); // save current context, and then switch to scheduler

    return 0;
};

/* terminate a thread */
//
void rpthread_exit(void *value_ptr){
    // Deallocated any dynamic memory created when starting this thread
    //we need to something to data structure
    //set to terminated
    //check any join threads
    //for each thread
    //set to ready
    //get value ptr_ptr=value ptr

    //add to finished/ wait queue
    //swap to scheduler

    // YOUR CODE HERE
};

/* Wait for thread termination */
// inside of tcb keep a list of all threads waiting to join
int rpthread_join(rpthread_t thread, void **value_ptr)
{
    //main calles join(1,)
    //T1  has an int * b pointing to 2
    //T1 will call exit and cast to (void*) b
    // now main needs to get the void* b so it needs a ptr to a ptr

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
    //go inside when thread finishes, timer goes off , or yield

    // Invoke different actual scheduling algorithms
    // according to policy (STCF or MLFQ)

    // if (sched == STCF)
    //		sched_stcf();
    // else if (sched == MLFQ)
    // 		sched_mlfq();

    while (1) //update tcb enq and deq you need while loop !!!
    {         // is the while loop calling the same sub rountine schedule func over and over?

        if (sctf_flag)
        {
            sched_stcf();
        }
        else
        {
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

// * UNDELETE THIS STUFF LATER !
// /*  Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf()
{
    // Your own implementation of STCF
    // (feel free to modify arguments and return types)
    //change states update quantum
    //call emqueue and dequence in some order that makes sense
    //set contect next thread swap

    //stfc enq increment time quantum put to the end of it LL
    //stcf deq find the shortest one find smallest quantum by looping thru list
    //deq stcf keep track of min quantum and return that one dont do anything else

    if (getQueueSize(ml_queue[0]->head) == 1)
    {
    }
    else
    {
    }

    // YOUR CODE HERE
    return; // ! delete later
}

// /* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq()
{
    // Your own implementation of MLFQ
    // (feel free to modify arguments and return types)
    //update curr priority by decrementing it

    // YOUR CODE HERE
    return; // ! delete later
}

// Feel free to add any other functions you need

// ! YOUR CODE HERE !

void create_scheduler_context()
{

    ucontext_t *uctx_sched = (ucontext_t *)malloc(sizeof(ucontext_t));

    /*create stack and check if memory allocation good*/
    void *stack_sched = malloc(STACK_SIZE);

    if (stack_sched == NULL)
    {
        perror("Failed to allocate stack");
        exit(1);
    }
    // use getcontext to avoid segfault
    if (getcontext(uctx_sched) < 0)
    {
        perror("getcontext");
        exit(1);
    }

    // set the necessary values

    uctx_sched->uc_stack.ss_sp = stack_sched;
    uctx_sched->uc_stack.ss_size = STACK_SIZE;
    uctx_sched->uc_stack.ss_flags = 0;
    uctx_sched->uc_link = NULL; // it will switch to that context, but since null will exit

    // make the context for future use
    makecontext(uctx_sched, (void *)&schedule, 0);
}

void create_tcb_main()
{
    tcb *tcb_main = (tcb *)malloc(sizeof(tcb));
    tcb_main->tid = 0;
    tcb_main->t_status = RUNNING;
    tcb_main->priority = 0;

    /*Creating the threadControlBlock(tcb)  */
    ucontext_t *uctx_main = (ucontext_t *)malloc(sizeof(ucontext_t));

    // save context of main
    if (getcontext(uctx_main) < 0)
    {
        perror("getcontext: create_tcb_main");
        exit(1);
    }
    uctx_current = uctx_main; // make current context point to main

    tcb_main->t_context = uctx_main;
}

void init_timer()
{
    memset(&mytime, 0, sizeof(mytime));
    mytime.it_value.tv_usec = 0;
    mytime.it_value.tv_sec = 0;
    mytime.it_interval.tv_usec = 0;
    mytime.it_interval.tv_sec = 0;
    setitimer(ITIMER_PROF, &mytime, NULL);
}

void enqueue(tcb *tcb_node)
{

    if (sctf_flag)
    {
        if (ml_queue[0]->head == NULL)
        {
            ml_queue[0]->head = tcb_node;
            ml_queue[0]->tail = tcb_node;
        }
        else
        {
            ml_queue[0]->tail->next = tcb_node;
            ml_queue[0]->tail = tcb_node;
        }
    }
    else
    { //MLFQ algo
        int quant = tcb_node->quantum;
        int level = get_level(quant);

        if (ml_queue[level] == NULL)
        {
            ml_queue[level]->head = tcb_node;
            ml_queue[level]->tail = tcb_node;
        }
        else
        {
            ml_queue[level]->tail->next = tcb_node;
            ml_queue[level]->tail = tcb_node;
        }
    }
}

tcb *tcb_init(ucontext_t *cctx, rpthread_t id)
{
    tcb *newNode = (tcb *)malloc(sizeof(tcb));
    newNode->tid = id;
    //NEED TO SET STATUS
    newNode->t_context = cctx;
    newNode->priority = 0; //default val
    //need stack?
    newNode->next = NULL;
    return newNode;
}
// YOUR CODE HERE

int getQueueSize(tcb *head)
{ //not super good method since has to pass whole time to check size
    tcb *curr = head;
    int size = 0;
    while (curr != NULL)
    {
        size++;
        curr = curr->next;
    }
    return size;
}

void printQueue(tcb *head)
{
    tcb *curr = head;

    while (curr != NULL)
    {
        printf("%d\t", curr->tid);
        curr = curr->next;
    }
    printf("\n"); //new line
}

tcb *dequeue()
{

    if (sctf_flag)
    {
        head = ml_queue[0]->head;
        if (getQueueSize(head) != 0)
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

    else
    { //MLFQ
        tcb *first;
        int i = 0;
        for (i; i < LEVELS; i++)
        {
            mlq *curr_q = ml_queue[i];
            if (curr_q->head != NULL)
            {
                first = curr_q->head;
                curr_q->head = curr_q->head->next;
                return first;
            }
        }
        puts("all levels are empty");
        return NULL;
    }
}

void ml_queue_init()
{

    if (sctf_flag == 0)
    {
        int i = 0;
        for (i; i < LEVELS; i++)
        {
            ml_queue[i] = (mlq *)malloc(sizeof(mlq));
            ml_queue[i]->head = NULL;
            ml_queue[i]->tail = NULL;
            i++;
        }
    }
    else
    {
        ml_queue[0] = (mlq *)malloc(sizeof(mlq));
        ml_queue[0]->head = NULL;
        ml_queue[0]->tail = NULL;
        //rest indexes should be null but will need to check
    }
}

int get_level(int quant)
{
    if (quant <= L1)
    {
        return 0;
    }
    else if (quant > L1 && quant <= L2)
    {
        return 1;
    }
    else if (quant > L2 && quant <= L3)
    {
        return 2;
    }
    else
    { //L4
        return 3;
    }
}