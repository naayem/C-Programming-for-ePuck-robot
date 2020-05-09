#include <motors_processing.h>

#define VITESSE_GENERALE  12

void motors_set_position(float position_l, float position_r, float speed_l, float speed_r){
	position_l = cm_to_steps(position_l);
	position_r = cm_to_steps(position_r);

	speed_l = cm_to_steps(speed_l);
	speed_r = cm_to_steps(speed_r);

	left_motor_set_speed(speed_l);
	right_motor_set_speed(speed_r);

	while((abs(right_motor_get_pos()) < abs(position_r)) || (abs(left_motor_get_pos()) < abs(position_l))){
		chThdSleepMilliseconds(2);
	}
	motors_stop_pos();
	return;
}

void moteurs_avance(void){
	left_motor_set_speed(cm_to_steps(VITESSE_GENERALE));
	right_motor_set_speed(cm_to_steps(VITESSE_GENERALE));
}

void motors_stop_pos(void){
	left_motor_set_speed(0);
	right_motor_set_speed(0);

	left_motor_set_pos(0);
	right_motor_set_pos(0);
}

void motors_stop_speed(void){
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

int32_t cm_to_steps(float valeur){
	int32_t steps = 0;
	steps = round(valeur*STEPS_PER_TURN/WHEEL_PERIMETER);
	return steps;
}

float steps_to_cm(float steps){
	float cm = 0;
	cm = (WHEEL_PERIMETER * steps / STEPS_PER_TURN);
	return cm;
}

void moteurs_tourne(float angle_rotation )
{
	float v_rot = 0;
	float p_rotation = (PERIMETER_EPUCK * angle_rotation / (2*PI));

	if (angle_rotation>0)
		v_rot = VITESSE_ROT;
	else v_rot = -VITESSE_ROT;

	motors_set_position(p_rotation, p_rotation, -v_rot, v_rot);
	//motors_stop_pos();
	update_map_angle(angle_rotation);
}
