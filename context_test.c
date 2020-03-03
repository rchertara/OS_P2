#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <ucontext.h>
#include <sys/time.h>
#include <string.h>

/*
Creating program that creates two ucontexts to run two functions, foo() 
and bar(). Within foo(), let it print out “foo” in a never ending while 
loop, and within bar() let it print out in a never ending while loop. 
Then use timers and swapcontext() to swap between the two threads of 
execution every 1 second.
*/




#define STACK_SIZE SIGSTKSZ
static ucontext_t *uctx_foo, *uctx_bar , uctx_main;
static volatile sig_atomic_t switch_context = 0;

void foo(){

	while(1){
		// printf("foo\n");
		if (switch_context)
		 {
		 	printf("foo_if\n");
		 	switch_context = 0;
		 	swapcontext(uctx_foo,uctx_bar);
			 printf("broke here foo\n");
		 } 
		// sleep(3);
	}
}
void bar(){

	while(1){
		// printf("bar\n");
		if (switch_context)
		 {
		 	printf("bar_if\n");
			struct itimerval local;
			getitimer(ITIMER_PROF, &local);
			memset(&local, 0 , sizeof(local));
		 	switch_context = 0;
            local.it_value.tv_usec = 0;
            local.it_value.tv_sec = 0;
            local.it_interval.tv_usec = 0; 
            local.it_interval.tv_sec = 0;
            setitimer(ITIMER_PROF, &local, NULL);
		 	// swapcontext(uctx_bar,uctx_foo);cle
			exit(0);
  
		 } 
	
	}
}

void sig_func(){

switch_context = 1;

	printf("signal handler worked\n");
}
int main(int argc, char **argv) {
	
	struct itimerval timer;
	if (argc != 1) {
		printf(": USAGE Program Name and no Arguments expected\n");
		exit(1);
	}
	setvbuf(stdout, 0, _IOLBF, 0);
	void *stack_foo=malloc(STACK_SIZE);
	void *stack_bar=malloc(STACK_SIZE);

	if (stack_foo == NULL || stack_bar == NULL){
		perror("Failed to allocate stack");
		exit(1);
	}
	uctx_foo= (ucontext_t*)malloc(sizeof(ucontext_t));
	if (getcontext(uctx_foo) < 0){
		perror("getcontext");
		exit(1);
	}
	uctx_foo->uc_stack.ss_sp = stack_foo;
	uctx_foo->uc_stack.ss_size = STACK_SIZE;
	uctx_foo->uc_stack.ss_flags = 0; 
	uctx_foo->uc_link  = NULL;
	makecontext(uctx_foo, (void *)&foo, 0);

	uctx_bar= (ucontext_t*)malloc(sizeof(ucontext_t));
	if (getcontext(uctx_bar) < 0){
		perror("getcontext");
		exit(1);
	}
	uctx_bar->uc_stack.ss_sp = stack_bar;
	uctx_bar->uc_stack.ss_size = STACK_SIZE;
	uctx_bar->uc_stack.ss_flags = 0; 
	uctx_bar->uc_link  = NULL;
	makecontext(uctx_bar, (void *)&bar,0);

	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = sig_func;
	// sa.sa_flags = SA_RESTART; do not set this
	sigaction (SIGPROF, &sa, NULL);

	// Create timer struct
	
	memset(&timer, 0 , sizeof(timer));
	
	
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 2;
    timer.it_interval.tv_usec = 0; 
    timer.it_interval.tv_sec = 2;
	setitimer(ITIMER_PROF, &timer, NULL);

	// 
	setcontext(uctx_foo);
	// foo();
	// while(1);
	// while
	
	return 0;
}