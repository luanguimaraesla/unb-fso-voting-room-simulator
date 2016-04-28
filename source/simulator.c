#include "simulator.h"
#include "officials.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

extern voting_room_controller *vrctl;

int create_and_set_shared_memory(void){
  key_t key = rand() % 9000 + 1000;
  if((shm_id = shmget(ftok("/tmp", key), sizeof(voting_room_controller), IPC_CREAT | 0666)) < 0){
    fprintf(stderr, "Error: shmget failure!\n");
    exit(1);
  }else{
    vrctl = (voting_room_controller *) shmat (shm_id, NULL, 0);
  }
}

int main(){
  int number_of_senators;
  int number_of_aldermen;
  int number_of_assemblymen;
  int *pids_sen;
  int *pids_ald;
  int *pids_ass;
  int counter_sen, counter_ald, counter_ass;
  int i, status;

  srand(time(NULL));
  number_of_senators = rand() % 10 + 1;  
  number_of_aldermen = rand() % 50 + 1;  
  number_of_assemblymen = rand() % 20 + 1;

  pids_sen = (int *) malloc (sizeof(int) * number_of_senators);
  pids_ald = (int *) malloc (sizeof(int) * number_of_aldermen);
  pids_ass = (int *) malloc (sizeof(int) * number_of_assemblymen);
  counter_sen = counter_ald = counter_ass = 0;

  create_and_set_shared_memory();
  set_voting_room_controller(vrctl);
  fprintf(stdout, "Senadores: %d, Deputados: %d, Vereadores: %d\n\n", number_of_senators, number_of_assemblymen, number_of_aldermen);
  sleep(1);

  

  while(counter_sen < number_of_senators){
    if((*(pids_sen + counter_sen) = fork()) < 0){
      fprintf(stderr, "Error: forking senator\n");
    }else if(*(pids_sen + counter_sen) == 0){
      enter_in_the_voting_room(counter_sen, senator);
      exit(0); 
    }else{
      counter_sen++;
      continue;
    }
  }

  while(counter_ald < number_of_aldermen){
    if((*(pids_ald + counter_ald) = fork()) < 0){
      fprintf(stderr, "Error: forking senator\n");
    }else if(*(pids_ald + counter_ald) == 0){
      enter_in_the_voting_room(counter_ald, alderman);      
      exit(0); 
    }else{
      counter_ald++;
      continue;
    }
  }

  while(counter_ass < number_of_assemblymen){
    if((*(pids_ass + counter_ass) = fork()) < 0){
      fprintf(stderr, "Error: forking senator\n");
    }else if(*(pids_ass + counter_ass) == 0){
      enter_in_the_voting_room(counter_ass, assemblyman);      
      exit(0); 
    }else{
      counter_ass++;
      continue;
    }
  }

  for(i = 0; i < number_of_senators; i++){
    waitpid(*(pids_sen + i), &status, 0);
  }
  for(i = 0; i < number_of_aldermen; i++){
    waitpid(*(pids_ald + i), &status, 0);
  }
  for(i = 0; i < number_of_assemblymen; i++){
    waitpid(*(pids_ass + i), &status, 0);
  }
  
  shmctl(shm_id, IPC_RMID, NULL);
  
  return 0;
}
