#ifndef GAME_MANAGEMENT_H_
#define GAME_MANAGEMENT_H_

#define ERROR_THRESHOLD			0.1f

typedef enum lettre{
	AUCUN=0,
	LETTRE_M,
	LETTRE_O,
	LETTRE_N,
	LETTRE_D,
	LETTRE_A,
	ELIOT,
	ALPHA_INIT
} lettre;

/**
 * Coordonnees du ePuck actualisees lors d'une trajectoire rectiligne � 100Hz,
 * a la fin de chaque rotation et de chaque trajectoire
*/
typedef struct mapping{
    float x;
    float y;
    float angle;
}mapping;

/**
 * Enumeration des differents etats de la machine d'etat qui regit les ordres que l'on peut donner a l'ePuck
 */
typedef enum order{
	ARRET=0,
	AVANCE,
	TOURNE
} order;

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
 * @brief Fixe et decide les transitions possibles d'un etat a l'autre de la machine d'etat des jeux.
 *
 * @param changeState			Next state request
 *
 */
void state_compare(etats changeState);

/**
 * @brief update l'angle de mapping du ePuck selon l'angle de rotation qu'on lui a demandé d'effectuer
 *
 * @param angle_rotation			Angle de rotation a effectué par l'ePuck
 *
 */
void update_map_angle(float angle_rotation);

#endif /* GAME_MANAGEMENT_H_ */





