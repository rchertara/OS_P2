#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../rpthread.h"

/* A scratch program template on which to call and
 * test rpthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 *
 *
 */
void context_test(int param){
    printf("%d",param);
}

int main(int argc, char **argv) {
    int i;
    for(i=0;i<3;i++) {
        void* param = i;
        pthread_t *thread = (pthread_t *) malloc(sizeof(pthread_t));
        rpthread_create(thread, NULL, &context_test, param);
    }
    return 0;
}
