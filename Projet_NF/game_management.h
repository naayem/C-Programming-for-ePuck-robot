#ifndef GAME_MANAGEMENT_H_
#define GAME_MANAGEMENT_H_

#include <stdio.h>

typedef enum lettre{
AUCUN=0,
LETTRE_M,
LETTRE_O,
LETTRE_N,
LETTRE_D,
LETTRE_A
} lettre;

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

/*en cas de dépassement d'un côté d'un joueur
 * 1 point pour l'autre et retour dans la direction initiale
 */
void sortie_gagnant(void);

/* permet au robot de se tourner dans la direction et sens
 * du point initial de départ
 */
void go_home(void);

//passer la semaphore du pong à d'autre module
void wait_image_needed(void);



#endif /* GAME_MANAGEMENT_H_ */



