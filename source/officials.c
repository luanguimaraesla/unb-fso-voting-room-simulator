#include "simulator.h"
#include "officials.h"
#include "voting_room.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef COLOR
#define COLOR

#define KNRM  "\x1B[0m"           // NORMAL
#define KRED  "\x1B[31m"          // RED
#define KGRN  "\x1B[32m"          // GREEN
#define KYLW  "\x1B[33m"          // YLW
#define KCYN  "\x1B[36m"          // CYN

#endif

voting_room_controller *vrctl; 

void vote(int official_id, official_type official){
  // turn green
  fprintf(stdout, "%s", KGRN);
  if(official == senator)
    fprintf(stdout, "[VOTING] Senator: %d\n", official_id);
  else if(official == alderman)
    fprintf(stdout, "[VOTING] Alderman: %d\n", official_id);
  else if(official == assemblyman)
    fprintf(stdout, "[VOTING] Assemblyman: %d\n", official_id);
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  }
  // turn normal
  fprintf(stdout, "%s", KNRM);
  usleep(rand() % 1000);
}

void think(int official_id, official_type official){
  // turn yellow
  fprintf(stdout, "%s", KYLW);
  if(official == senator)
    fprintf(stdout, "[THINKING] Senator: %d\n", official_id);
  else if(official == alderman)
    fprintf(stdout, "[THINKING] Alderman: %d\n", official_id);
  else if(official == assemblyman)
    fprintf(stdout, "[THINKING] Assemblyman: %d\n", official_id);
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  } 
  // turn normal
  fprintf(stdout, "%s", KNRM);
  usleep(rand() % 1000);
}

void exit_room(int official_id, official_type official){
  // turn red
  fprintf(stdout, "%s", KRED);
  if(official == senator)
    fprintf(stdout, "[EXITING] Senator: %d\n", official_id);
  else if(official == alderman)
    fprintf(stdout, "[EXITING] Alderman: %d\n", official_id);
  else if(official == assemblyman)
    fprintf(stdout, "[EXITING] Assemblyman: %d\n", official_id);
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  } 
  // turn normal
  fprintf(stdout, "%s", KNRM);
  usleep(rand() % 50);
}

void increment_official_counter(official_type official){
  sem_wait(&(vrctl->check_counter_mutex));
  (vrctl->official_counter)++;
  
  if(official == senator) (vrctl->number_of_senators)++;
  else if(official == alderman) (vrctl->number_of_aldermen)++;
  else if(official == assemblyman) (vrctl->number_of_assemblymen)++;
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  }
 
  sem_post(&(vrctl->check_counter_mutex));
}

void decrement_official_counter(official_type official){
  sem_wait(&(vrctl->check_counter_mutex));
  (vrctl->official_counter)--;
  
  if(official == senator) (vrctl->number_of_senators)--;
  else if(official == alderman) (vrctl->number_of_aldermen)--;
  else if(official == assemblyman) (vrctl->number_of_assemblymen)--;
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  }

  sem_post(&(vrctl->check_counter_mutex));
}

void enter_in_the_voting_room(int official_id, official_type official){
  think(official_id, official);

  if(official == senator){
    while(1){
      // Block the voting room controller's variables access
      sem_wait(&(vrctl->check_counter_mutex));
      // If there is no officials into the voring room,
      // the senator can vote, note the mutex still blocked
      if(vrctl->official_counter == 0) break;
      // Else, unlock the mutex and continue looping
      else sem_post(&(vrctl->check_counter_mutex));  
    }
    // Increment voting room controller's variables
    (vrctl->official_counter)++;
    (vrctl->number_of_senators)++;
    
    vote(official_id, official); // vote
  
    // Decrement voting room controller's variables
    exit_room(official_id, official);
    (vrctl->official_counter)--;
    (vrctl->number_of_senators)--;
    // Unlock the mutex
    sem_post(&(vrctl->check_counter_mutex));  
  }

  else if(official == alderman){
    increment_official_counter(official);
    vote(official_id, official);
    exit_room(official_id, official);
    decrement_official_counter(official);
  }
  else if(official == assemblyman){
    sem_wait(&(vrctl->assemblyman_counter));
    increment_official_counter(official);
    vote(official_id, official);
    exit_room(official_id, official);
    decrement_official_counter(official);
    sem_post(&(vrctl->assemblyman_counter)); 
  }
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  } 
}

void create_officials(int *pids, int number_of_officials, official_type type){
  int counter = 0;

  while(counter < number_of_officials){
    if((*(pids + counter) = fork()) < 0){
      fprintf(stderr, "Error: forking official.\n");
    }else if(*(pids + counter) == 0){
      enter_in_the_voting_room(counter, type);
      exit(0); 
    }else{
      counter++;
      continue;
    }
  }
}
