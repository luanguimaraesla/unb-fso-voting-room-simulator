#ifndef VOTING_ROOM
#define VOTING_ROOM

#include "voting_room.h"
#include <semaphore.h>

struct voting_room_controller{
  sem_t check_counter_mutex;
  int official_counter;
  sem_t assemblyman_counter;
  int number_of_senators;
  int number_of_assemblymen;
  int number_of_aldermen;
};

typedef struct voting_room_controller voting_room_controller;

void set_voting_room_controller(voting_room_controller *attr);

#endif
