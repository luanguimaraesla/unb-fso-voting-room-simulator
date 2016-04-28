#include "simulator.h"
#include "officials.h"
#include "voting_room.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

voting_room_controller *vrctl; 

void vote(int official_id, official_type official){
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
}

void think(int official_id, official_type official){
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
  usleep(rand() % 1000000);
}

void increment_official_counter(void){
  sem_wait(&(vrctl->check_counter_mutex));
  (vrctl->official_counter)++;
  sem_post(&(vrctl->check_counter_mutex));
}

void decrement_official_counter(void){
  sem_wait(&(vrctl->check_counter_mutex));
  (vrctl->official_counter)--;
  sem_post(&(vrctl->check_counter_mutex));
}

void enter_in_the_voting_room(int official_id, official_type official){
  think(official_id, official);

  if(official == senator){
    while(1){
      sem_wait(&(vrctl->check_counter_mutex));
      if(vrctl->official_counter == 0) break;
      else sem_post(&(vrctl->check_counter_mutex));  
    }
    (vrctl->official_counter)++;
    vote(official_id, official);
    (vrctl->official_counter)--;
    sem_post(&(vrctl->check_counter_mutex));  
  }

  else if(official == alderman){
    increment_official_counter();
    vote(official_id, official);
    decrement_official_counter();
  }
  else if(official == assemblyman){
    sem_wait(&(vrctl->assemblyman_counter));
    increment_official_counter();
    vote(official_id, official);
    decrement_official_counter();
    sem_post(&(vrctl->assemblyman_counter)); 
  }
  else{
    fprintf(stderr, "Error, invalid official_type!\n");
    exit(1);
  } 
}
