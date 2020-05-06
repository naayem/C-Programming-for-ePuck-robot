#ifndef IR_THREAD_H_
#define IR_THREAD_H_

#include <main.h>
#include <sensors/proximity.h>

/* Test de proximité avec les 2 capteurs IR à l'avant du robot
 * Renvoie 1 en cas de feuille proche et 0 sinon
 */
_Bool aide_detection_ligne(void);

/* Test d'un obstacle du côté droit de l'e-puck
 *
 * int reflet_obstacle - le niveau de reflet prévu pour l'obstacle
 */
_Bool obstacle_droite(int reflet_obstacle);

/* Test d'un obstacle du côté gauche de l'e-puck
 *
 * int reflet_obstacle - le niveau de reflet prévu pour l'obstacle
 */
_Bool obstacle_gauche(int reflet_obstacle);

#endif
