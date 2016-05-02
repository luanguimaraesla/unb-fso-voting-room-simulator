#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
pthread_mutex_t mutex;
sem_t s[N];
pthread_t fil[N];

void test(int i){
  if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
    state[i] = EATING;
    sem_post(&(s[i]));
  } 
}

void think(int i){
  printf("%d: THINKING\n", i);
}

void eat(int i){
  pthread_mutex_lock(&mutex);
  if(state[i] == EATING)
    printf("%d: EATING\n", i);
  pthread_mutex_unlock(&mutex);
}

void take_forks(int i){
  pthread_mutex_lock(&mutex);
  state[i] = HUNGRY;
  test(i);
  pthread_mutex_unlock(&mutex);
  sem_wait(&(s[i])); 
}

void put_forks(int i){
  pthread_mutex_lock(&mutex);
  state[i] = THINKING;
  test(LEFT);
  test(RIGHT);
  pthread_mutex_unlock(&mutex);
}

void * filosofo(void * i_aux){
  int i = (int) i_aux;
  
  while(1){
    think(i);
    sleep(1);
    take_forks(i);
    eat(i);
    put_forks(i);
  }

  pthread_exit(NULL);
}

int main(){
  pthread_mutex_init(&mutex, NULL);
  int a;
  for(a = 0; a < N; a++)
    sem_init(&(s[a]), 0, 0);
  
  for(a = 0; a < N; a++)
    pthread_create(&fil[a], NULL, filosofo, (void *) a);

  for(a = 0; a < N; a++)
    pthread_join(fil[a], NULL);
  
  
  for(a = 0; a < N; a++)
    sem_destroy(&(s[a]));
  pthread_mutex_destroy(&mutex);

}
