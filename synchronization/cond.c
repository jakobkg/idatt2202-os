#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int cnt = 0;
int a[]={1,2,3,4,5,6,7,8,9};
char arr[]={'a','b','c','d','e','f','g','h','j'};
sem_t s1;
void* pc() {
    int i=0;
    while(i<9) {
        pthread_mutex_lock(&mut);
        while (cnt==0)
            pthread_cond_wait(&cond, &mut);
        printf("%c ", arr[i++]);
        cnt=0;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}
void* pi() {
    int i=0;
    while(i<9) {
        pthread_mutex_lock(&mut);
        printf("%d ",a[i++]);
        cnt++;
        pthread_cond_broadcast(&cond);
        while (cnt==1)
            pthread_cond_wait(&cond, &mut);
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}
int main() {
    pthread_t t1,t2;
    pthread_cond_init(&cond, NULL);
    pthread_create(&t1, NULL, pc, NULL);
    pthread_create(&t2, NULL, pi, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\n");
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mut);
    return 0;
}
