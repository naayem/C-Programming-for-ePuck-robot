#ifndef MOTORS_PROCESSING_H_
#define MOTORS_PROCESSING_H_

#include <motors.h>
#include <math.h>
#include <arm_math.h>

#include <main.h>
#include <game_management.h>




void motors_set_position(float position_l, float position_r, float speed_l, float speed_r);
void motors_stop_speed(void);
void motors_stop_pos(void);

void moteurs_avance(void);



int32_t cm_to_steps(float valeur);
float steps_to_cm(float steps);

order change_order(order previous_state, order new_state);



#endif /* MOTORS_PROCESSING_H_ */
