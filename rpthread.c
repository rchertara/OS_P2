// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "rpthread.h"

/* INITAILIZE ALL YOUR VARIABLES HERE*/
//main has tcb, then current thing we should make it a tcb, scheduler does not need a tcb just a global var
// tcb *head , *tail; //  > might not be able to create global head and tail
// .*head = *tail = NULL;

boolean first_time_creating, sctf_flag ;    // variable used to check if pthread_create has ever been run before
first_time_creating = TRUE; sctf_flag = FALSE; 

ucontext_t *uctx_sched; // current thread context; scheduler context
tcb* current_thread_tcb;//! julian you need to do something with this cap
mlq *ml_queue[4] , *queue_waiting_to_join; // TODO allocate memory 
struct itimerval mytime; // TODO allocate memory 

exited_threads_LL * exited_threads; // TODO allocate memory 
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
    current_thread_tcb->t_context=READY;
    getitimer(ITIMER_PROF, &mytime);
    mytime.it_value.tv_usec = 0; // stop the timer
    setitimer(ITIMER_PROF, &mytime, NULL);
    swapcontext(current_thread_tcb->t_context, uctx_sched); // save current context, and then switch to scheduler

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

    tcb *ptr_current = current_thread_tcb;
    tcb * ptr_tcb = search_for_tid(thread);
    
    //DID NOT FIND THE THREAD IN THE MAIN QUEUE
    if(ptr_tcb == NULL){
        // ! FIGURE OUT DATA STRUCTURE FOR RETURN VALUES 
        exited_threads_LL * ptr = exited_threads->head;
        exited_threads_LL * prev = NULL;

        while(ptr!=NULL){
            if ( ptr->finished_thread->tid == thread ) break;
            prev = ptr;
            ptr = ptr->next;
                
        }

        if(ptr == NULL ) return; // SOMEThiNG WENT WRONG AND nEVER FOUND

        /* Handling the case of FIRST ELEMENT being joined and  ONLY ELEMENT*/
        if(prev == NULL && ptr->next ==NULL){
            *value_ptr = ptr->return_values;
            free(ptr); 
            return;
        }
        /* Handling the case of FIRST ELEMENT being joined and MORE ELEMENTs in LIST*/
        if(prev == NULL ptr->next !=NULL ){
            *value_ptr = ptr->return_values;
            exited_threads->head = exited_threads->head->next; 
            ptr->next = NULL; 
            free(ptr); 
            return; 
        }
        /* FINALLY: Handle the case where its some arbitrary NODE in the list*/
        *value_ptr = ptr->return_values;
        prev->next = ptr->next; 
        ptr->next = NULL; 
        free(ptr); 
        return; 

    }

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


        tcb * schedule_thread= dequeue();
        schedule_thread->quantum++;
        schedule_thread->t_status=RUNNING;
        swapcontext(uctx_sched,schedule_thread->t_context);

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
    //uctx_current = uctx_main; // make current context point to main

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



tcb* get_shortestJob(tcb * head){
    if(head==NULL){
        puts("Nothing in this Queue");
        return NULL;
    }
    tcb *curr = head;
    tcb *shortest_job=NULL;
    int min_quantum=INT64_MAX;

    while(curr!=NULL){
        if(curr->quantum < min_quantum){
            min_quantum=curr->quantum;
            shortest_job=curr;
        }
        curr=curr->next;
    }
    return shortest_job;
}


tcb *dequeue()
{
    // * FALSE == 0 , TRUE = anything other than 0, in this case 1 
    if (sctf_flag)
    {
        tcb * shortest_job=get_shortestJob(ml_queue[0]->head);
        //do i have to check its state for both algos to make sure i can use it???
        return shortest_job;
    }

    else
    { //MLFQ
        int i = 0;
        for (i; i < LEVELS; i++)
        {
            mlq *curr_q = ml_queue[i];
            tcb * shortest_job=get_shortestJob(curr_q->head);
            if(shortest_job!=NULL){
                return shortest_job;
            }
            else{
                puts("Nothing on this Lvl");
            }
        }

        return NULL;

    }
}

void ml_queue_init()
{
    // * FALSE == 0 , TRUE = anything other than 0, in this case 1 
    if (sctf_flag )
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

tcb * search_for_tid(rpthread_t goal_tid){
    tcb * ptr_queue_head;
    
    if(sctf_flag){
        ptr_queue_head = ml_queue[0]-> head;

        while(ptr_queue_head!=NULL){
            if(ptr_queue_head ->tid == goal_tid) return ptr_queue_head; // found goal_tid in queue 
            ptr_queue_head = ptr_queue_head->next;
        }
    }
    else{
        int curr_level = 0;
        while(curr_level < LEVELS){
            ptr_queue_head = ml_queue[curr_level]-> head;
               while(ptr_queue_head!=NULL){
                    if(ptr_queue_head ->tid == goal_tid) return ptr_queue_head; // found goal_tid in queue 
                    ptr_queue_head = ptr_queue_head->next;
            }
            // go to next level
            curr_level++;
        }
    }

    // Did not find goal_tid
    return NULL;
}