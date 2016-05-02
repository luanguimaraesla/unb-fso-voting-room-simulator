#include "simulator.h"
#include "officials.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <pthread.h>

extern voting_room_controller *vrctl;

typedef struct pthread_create_officials_args {
  int number_of_officials;
  int *pids;
  official_type type;
} officials_args;

void * thread_create_officials(void *args){
  officials_args *cast_args = (officials_args *) args;
  create_officials(cast_args->pids, cast_args->number_of_officials, cast_args->type);
  pthread_exit(NULL);
}

officials_args * create_officials_args(int *pids, int number_of_officials, official_type type){
  officials_args *new = (officials_args *) malloc (sizeof(officials_args));
  new->pids = pids;
  new->number_of_officials = number_of_officials;
  new->type = type;

  return new;
}

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
  int number_of_senators, number_of_aldermen, number_of_assemblymen; // Number of processes
  int *pids_sen, *pids_ald, *pids_ass; // Official's processes PID's
  pthread_t pth_senators, pth_aldermen, pth_assemblymen;
  officials_args *senators_args, *aldermen_args, *assemblymen_args;
  int i, status; // Generic counter and IPC status variable


  // Random generate for the number of officials
  srand(time(NULL));
  number_of_senators = rand() % 10 + 1;  
  number_of_aldermen = rand() % 20 + 1;  
  number_of_assemblymen = rand() % 15 + 1;

  // Create an array for each official type
  pids_sen = (int *) malloc (sizeof(int) * number_of_senators);
  pids_ald = (int *) malloc (sizeof(int) * number_of_aldermen);
  pids_ass = (int *) malloc (sizeof(int) * number_of_assemblymen);

  // Create threads args
  senators_args = create_officials_args(pids_sen, number_of_senators, senator);
  aldermen_args = create_officials_args(pids_ald, number_of_aldermen, alderman);
  assemblymen_args = create_officials_args(pids_ass, number_of_assemblymen, assemblyman);
 
  // Initialize some necessary elements like
  // shared memory to share between processes the
  // struct voting_room_controller that contains
  // all the sempaphores to get this program working well
 
  create_and_set_shared_memory();
  set_voting_room_controller(vrctl);

  // Print the number of random created officials
  fprintf(stdout, "Senadores: %d, Deputados: %d, Vereadores: %d\n\n", number_of_senators, number_of_assemblymen, number_of_aldermen);
  sleep(1);

  // Generate each official's process:
  // Senators
  pthread_create(&pth_senators, NULL, thread_create_officials, senators_args);
  // Assemblymen
  pthread_create(&pth_assemblymen, NULL, thread_create_officials, assemblymen_args);
  // Aldermen
  pthread_create(&pth_aldermen, NULL, thread_create_officials, aldermen_args);

  // Wait for the threads
  pthread_join(pth_senators, NULL);
  pthread_join(pth_assemblymen, NULL);
  pthread_join(pth_aldermen, NULL);

  // Wait for all children finishing
  for(i = 0; i < number_of_senators; i++)
    waitpid(*(pids_sen + i), &status, 0);
  for(i = 0; i < number_of_aldermen; i++)
    waitpid(*(pids_ald + i), &status, 0);
  for(i = 0; i < number_of_assemblymen; i++)
    waitpid(*(pids_ass + i), &status, 0);
  
  shmctl(shm_id, IPC_RMID, NULL);
  
  return 0;
}
