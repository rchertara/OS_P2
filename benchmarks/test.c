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
    while(i<10000){
        i++;
    }
    rpthread_exit(NULL);
}

int main(int argc, char **argv) {
    int i;

    thread = (pthread_t*)malloc(6*sizeof(pthread_t));
    printf("%d \n",sizeof(pthread_t));
    counter = (int*)malloc(sizeof(int));
    int zz = 5;
    counter = &zz;

    for (i = 0; i < 5; i++) {

        pthread_create(&thread[i], NULL, &context_test, &counter[0]);
        printf("thread[%d] = %d\n", i,thread[i]);
    }
    for (int j = 0; j < 5; j++) {

        rpthread_join(thread[j],NULL);
//        printf("thread[%d] = %d\n", j,thread[j]);

    }

//
    free(thread);
//    free(counter);
    return 0;
}
