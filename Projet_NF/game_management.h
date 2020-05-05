#ifndef GAME_MANAGEMENT_H_
#define GAME_MANAGEMENT_H_


/**
 * Coordonnées du ePuck actualisé chaque X temps, a la fin de chaque rotation et de chaque trajectoire
*/
typedef struct mapping{
    float x;
    float y;
    float angle;
}mapping;

/**
 * Enumération des différents états de la machine d'état qui régit les ordres que l'on peut donner a l'ePuck
 */
typedef enum order{
	ARRET=0,
	AVANCE,
	TOURNE
} order;



//TO ADJUST IF NECESSARY. NOT ALL THE E-PUCK2 HAVE EXACTLY THE SAME WHEEL DISTANCE
#define WHEEL_DISTANCE      5.35f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)
#define VITESSE_ROT 			5
#define DEUX 			2
#define ZERO				0
#define DEG_TO_RAD	PI/180


/**
 * @brief Debute le thread qui s'occupe de mettre a jour la position du robot lorsqu'il est en trajectoire droite.
 */
void mapping_start(void);

/**
 * @brief Fonction principal du programme, du main. Il regit la machine d'état principal qui decide le mode de jeux,
 * 		  les evenements et les decisions.
 */
void management(void);


/**
 * @brief Fixe et decide les transitions possibles d'un état a l'autre de la machine d'état des jeux.
 *
 * @param changeState			Next state request
 *
 */
void state_compare(etats changeState);


/**
 * @brief Appel de semaphore du module camera processing
 *
 */
void wait_image_needed(void);

/**
 * @brief update l'angle de mapping du ePuck selon l'angle de rotation qu'on lui a demandé d'effectuer
 *
 * @param angle_rotation			Angle de rotation a effectué par l'ePuck
 *
 */
void update_map_angle(float angle_rotation);



#endif /* GAME_MANAGEMENT_H_ */



