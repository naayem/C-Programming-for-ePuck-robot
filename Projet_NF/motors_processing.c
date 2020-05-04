#include <motors_processing.h>





#define NSTEP_ONE_TURN      1000 // number of step for 1 turn of the motor
#define WHEEL_PERIMETER     13 // [cm]
#define STEPS_PER_TURN      1000


void motors_set_position(float position_l, float position_r, float speed_l, float speed_r){
	int boucle=0;
	int i=0;

	position_l = cm_to_steps(position_l);
	position_r = cm_to_steps(position_r);

	speed_l = cm_to_steps(speed_l);
	speed_r = cm_to_steps(speed_r);

	left_motor_set_speed(speed_l);
	right_motor_set_speed(speed_r);

	while(boucle==0){

		if ((abs(right_motor_get_pos()) > abs(position_r)) ) {
				motors_stop_pos();
				return;
		}else if ((abs(left_motor_get_pos()) > abs(position_l))) {
			motors_stop_pos();
			return;
		} else {
			i++; //QUOI FAIRE
		}

	}

}

void moteurs_avance(void){

	left_motor_set_speed(cm_to_steps(6));
	right_motor_set_speed(cm_to_steps(6));

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

int32_t cm_to_steps(float valeur)
{
	int32_t steps = 0;
	steps = round(valeur*STEPS_PER_TURN/WHEEL_PERIMETER);
	return steps;
}

float steps_to_cm(float steps)
{
	float cm = 0;
	cm = (WHEEL_PERIMETER * steps / STEPS_PER_TURN);
	return cm;
}


order change_order(order previous_state, order next_state){

	float total_steps = 0;

	if(previous_state == ARRET){
			if (next_state == AVANCE || next_state == TOURNE || next_state == ARRET){
				if(right_motor_get_pos() || left_motor_get_pos()){
					chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n");
				}
				return next_state;
			}
	}else if( previous_state == AVANCE){

			if( next_state == TOURNE ||  next_state == ARRET ){
				total_steps = (right_motor_get_pos()+left_motor_get_pos())/2;
				update_map_position();
				total_steps =0;

				right_motor_set_pos(0);
				left_motor_set_pos(0);
				return next_state;
			} else if(next_state == AVANCE)
				chprintf((BaseSequentialStream *)&SD3,"AVANCE deja en cours\n");
	}else if( previous_state == TOURNE){

			if( next_state == TOURNE){
				chprintf((BaseSequentialStream *)&SD3,"TOURNE deja en cours\n");
				return next_state;
			} else if(next_state == AVANCE)
				chprintf((BaseSequentialStream *)&SD3,"Refus de AVANCE car TOURNE deja en cours\n");
	}else if (next_state == ARRET){
		chprintf((BaseSequentialStream *)&SD3,"Tentative d'ARRET alors que TOURNE en cours!! previous=%d , next=%d\n", previous_state, next_state); //Tentative d'ARRET alors que TOURNE en cours
		//QUOI FAIRE?
	}




	else{
		chprintf((BaseSequentialStream *)&SD3,"ERROR aucune condition de change order n'a ete pris en compte\n!!! previous=%d , next=%d\n", previous_state, next_state);
	return previous_state;
	}
	return 0;
}



