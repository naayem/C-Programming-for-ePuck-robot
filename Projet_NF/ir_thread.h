#ifndef IR_THREAD_H_
#define IR_THREAD_H_

#include <main.h>
#include <sensors/proximity.h>


/* Test de proximité des 4 capteurs IR à l'avant du robot
 * Renvoie 1 en cas d'obstacle proche et 0 sinon*/

_Bool obstacle(void);

#endif /* IRT_HREAD_H_ */
