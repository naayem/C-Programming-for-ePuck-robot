#ifndef IR_THREAD_H_
#define IR_THREAD_H_

#include <main.h>
#include <sensors/proximity.h>

/* Test de proximité avec les 2 capteurs IR à l'avant du robot
 * Renvoie 1 en cas de feuille proche et 0 sinon
 */
_Bool aide_detection_ligne(void);

/* Test d'un obstacle devant de l'e-puck
 * Renvoie 1 en cas d'obstacle proche et 0 sinon
 */
_Bool obstacle_demi_tour (void);

#endif
