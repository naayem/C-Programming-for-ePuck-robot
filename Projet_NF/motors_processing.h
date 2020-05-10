#ifndef MOTORS_PROCESSING_H_
#define MOTORS_PROCESSING_H_

#include <motors.h>
#include <math.h>
#include <arm_math.h>

#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define WHEEL_PERIMETER     13 // [cm]
#define STEPS_PER_TURN      1000
#define WHEEL_DISTANCE      5.365f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)
#define VITESSE_ROT 		12 //cm/s
#define DEG_TO_RAD			PI/180
#define VITESSE_GENERALE  	12 //cm/s

/**
 * @brief Debute le thread qui s'occupe de mettre a jour la position du robot lorsqu'il est en trajectoire droite.
 *
 * @param position_l			position a atteindre du moteur gauche en cm.
 * @param position_r			position a atteindre du moteur droit en cm.
 * @param speed_l				vitesse du moteur gauche en cm/seconde.
 * @param speed_r				vitesse du moteur droit en cm/seconde.
 *
 */
void motors_set_position(float position_l, float position_r, float speed_l, float speed_r);

/**
 * @brief Change la vitesse des moteurs a Zero. STOP les moteurs.
 */
void motors_stop_speed(void);

/**
 * @brief Change la vitesse des moteurs a zero et replace les compteurs de pas interne au module des moteurs a zero.
 */
void motors_stop_pos(void);

/**
 * @brief Change la vitesse des moteurs a VITESSE_GENERAL.
 */
void moteurs_avance(void);

/**
 * @brief Realise la conversion de cm en nombre de pas moteurs.
 *
 * @param valeur			distance en cm a convertir en nombre de pas moteurs
 *
 */
int32_t cm_to_steps(float valeur);

/**
 * @brief Realise la conversion de pas moteurs en cm. Retourne le resultat en cm.
 *
 *  @param steps			nombre de pas moteurs a convertir en cm
 *
 */
float steps_to_cm(float steps);

/**
 * @brief Effectue une rotation de l'ePuck
 *
 * @param angle_rotation			Angle de rotation a effectuer par l'ePuck
 *
 */
void moteurs_tourne(float angle_rotation);

#endif /* MOTORS_PROCESSING_H_ */
