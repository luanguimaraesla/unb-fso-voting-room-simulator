#ifndef SIMULATOR
#define SIMULATOR

#include "voting_room.h"
#include "officials.h"

struct pthread_create_officials_args {
  int number_of_officials;
  int *pids;
  official_type type;
};

typedef struct pthread_create_officials_args officials_args;

// global variables
int shm_id;

// function's headers
void * thread_create_officials(void *args);
officials_args * create_officials_args(int *pids, int number_of_officials, official_type type);
int create_and_set_shared_memory(void);

#endif
