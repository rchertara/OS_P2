#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../rpthread.c"

/* A scratch program template on which to call and
 * test rpthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 *
 *
 */

int *counter;
pthread_t *thread;

void context_test(void* arg){
    int i=*((int*) arg);
    printf("i = %d\n",i);
    while(i<1000000){
        i++;
    }
    rpthread_exit(NULL);
}

int main(int argc, char **argv) {
    int i;
    int thread_num=5;
    thread = (pthread_t*)malloc(6*sizeof(pthread_t));
    printf("%d \n",sizeof(pthread_t));
    counter = (int*)malloc(sizeof(int));
    int zz = 5;
    counter = &zz;
    int j=0;
    for (i = 0; i < thread_num; i++) {

        pthread_create(&thread[i], NULL, &context_test, &counter[0]);
        printf("thread[%d] = %d\n", i,thread[i]);
    }
    for (j = 0; j < thread_num; j++) {

        rpthread_join(thread[j],NULL);
//        printf("thread[%d] = %d\n", j,thread[j]);

    }

//
    free(thread);
//    free(counter);
    return 0;
}
