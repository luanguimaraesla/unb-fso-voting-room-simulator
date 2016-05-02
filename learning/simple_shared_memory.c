#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>

typedef struct shared{
  int go;
  int msg;
  sem_t message_to_read;
  sem_t can_write;
}sh;

int main(){
  key_t key;
  int shm_id;
  sh *p_shm;

  srand(time(NULL));
  key = rand() % 9000 + 1000;
  
  //criando um segmento de memória compartilhada
  shm_id = shmget(ftok("/tmp", key), sizeof(sh), IPC_CREAT | 0666);

  //apontando para memória compartilhada
  p_shm = (sh *) shmat(shm_id, NULL, 0);
  sem_init(&(p_shm->can_write), 1, 1);
  sem_init(&(p_shm->message_to_read), 1, 0);
  p_shm->go = 10;

  int pid;
  pid = fork();

  if(pid == 0){
    //processo filho
    while(p_shm->go){
      sem_wait(&(p_shm->message_to_read));
      fprintf(stdout, "MSG = %d\n", p_shm->msg);
      sem_post(&(p_shm->can_write));
      sleep(1);
    }
    exit(0);
  }else{
    //processo pai
    while((p_shm->go)){
      sem_wait(&(p_shm->can_write));
      p_shm->msg = rand() % 9000 + 1000;
      (p_shm->go)--;
      sem_post(&(p_shm->message_to_read));
    }
  }

  int status;
  waitpid(pid, &status, 0);
  sem_destroy(&(p_shm->message_to_read));
  sem_destroy(&(p_shm->can_write));
  //delete shm
  shmctl(shm_id, IPC_RMID, 0);  

  return 0;
}
