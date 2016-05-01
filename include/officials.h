#ifndef OFFICIALS
#define OFFICIALS

enum official_type{
  assemblyman, alderman, senator
};

typedef enum official_type official_type;

void vote(int official_id, official_type official);
void think(int official_id, official_type official);
void enter_in_the_voting_room(int official_id, official_type official);
void increment_official_counter(void);
void decrement_official_counter(void);
void create_officials(int *pids, int number_of_officials, official_type type);

#endif
