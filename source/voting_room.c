#include "voting_room.h"
#include <stdlib.h>
#include <semaphore.h>

void set_voting_room_controller(voting_room_controller *attr){
  sem_init(&(attr->check_counter_mutex), 1, 1);
  sem_init(&(attr->assemblyman_counter), 1, 5);
  attr->official_counter = 0;
  attr->number_of_senators = 0;
  attr->number_of_assemblymen = 0;
  attr->number_of_aldermen = 0;
}
