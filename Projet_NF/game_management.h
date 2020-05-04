#ifndef GAME_MANAGEMENT_H_
#define GAME_MANAGEMENT_H_

#include <stdio.h>



typedef struct mapping{
    float x;
    float y;
    float angle;
}mapping;

typedef enum order{
	ARRET=0,
	AVANCE,
	TOURNE
} order;

void mapping_start(void);
void management(etats* currentState);

void state_compare(etats changeState);

void update_map_position(void);




void moteurs_tourne(float angle_rotation);
void update_map_angle(float angle_rotation);

void nouvel_ordre(order next_order, float angle_rotation);

void postAvance_init(void);

void boite_virtuelle(void);

void go_home(void);




#endif /* GAME_MANAGEMENT_H_ */



