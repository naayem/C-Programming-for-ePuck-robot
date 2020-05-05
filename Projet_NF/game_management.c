#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <main.h>

#include <math.h>

#include <arm_math.h>

#include <audio_processing.h>
#include <game_management.h>
#include <ir_thread.h>
#include <motors_processing.h>
#include <camera_processing.h>
#include <letter_writing.h>
#include <leds.h>

//TO ADJUST IF NECESSARY. NOT ALL THE E-PUCK2 HAVE EXACTLY THE SAME WHEEL DISTANCE
#define WHEEL_DISTANCE      5.35f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)
#define VITESSE_ROT 			5
#define DEUX 			2
#define ZERO				0
#define DEG_TO_RAD	PI/180

//semaphore
static BSEMAPHORE_DECL(image_needed_sem, TRUE);

static mapping ePuck = {0,0,0};


static etats nextState = 0;

static etats currentStateManagement = 0;

static order action = ARRET;

static float counted_current_steps = 0;
static uint8_t point_joueur_avant = 0, point_joueur_arriere = 0;

static THD_WORKING_AREA(waThdMapping, 628);
static THD_FUNCTION(ThdMapping, arg) {


    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    volatile systime_t time;





    while(1){

    			time = chVTGetSystemTime();

    			if(action==AVANCE){

    				//chSysLock();
    				update_map_position();
    				//chSysUnlock();


    			}

    			chprintf((BaseSequentialStream *)&SD3,"update x=%f .... y=%f .... angle=%f  \n", ePuck.x, ePuck.y, ePuck.angle);

			chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void mapping_start(void){
chThdCreateStatic(waThdMapping, sizeof(waThdMapping), NORMALPRIO, ThdMapping, NULL);
}


void management(etats* currentState){

	currentStateManagement = *currentState;


	//we create variables for the led in order to turn them off at each loop and to
	//select which one to turn on
	uint8_t led1 = 0, led3 = 0, led5 = 0, led7 = 0;
	posLine pong;


	switch (*currentState) {
	   case MENU_PRINCIPAL:
		   // do something in the idle state


		   led1 = 1;
		   led3 = 0;
		   led5 = 0;
		   led7 = 0;

		    palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
		    palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
		    palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
		    palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

		    *currentState = nextState;
		    currentStateManagement = *currentState;

		   break;

		case PONG_INIT:
		   // do something in the stop state

			//initialization of variables
			//activate threads

			proximity_start();
			calibrate_ir();

			led1 = 0;
			led3 = 1;
			led5 = 0;
			led7 = 1;

			palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
			palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
			palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
			palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

			//int angle_random = rand()%360;
			//nouvel_ordre( TOURNE,  angle_random*DEG_TO_RAD);
			nouvel_ordre( AVANCE,  0);

			*currentState = PONG;
			currentStateManagement = PONG;

		   break;

		case PONG:
			led1 = 1;
			led3 = 0;
			led5 = 0;
			led7 = 0;

			palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
			palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
			palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
			palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

			chBSemSignal(&image_needed_sem);
			if ((obstacle_droite(300))||(obstacle_gauche(300))){
				nouvel_ordre( TOURNE,  180*DEG_TO_RAD);
				nouvel_ordre( AVANCE,  0);
			}

			pong = close_line();

			switch(pong){
				case L_NULL:
					break;
				case L_DROITE:
					nouvel_ordre( TOURNE,  135*DEG_TO_RAD);
					nouvel_ordre( AVANCE,  0);
					break;
				case L_GAUCHE:
					nouvel_ordre( TOURNE,  -135*DEG_TO_RAD);
					nouvel_ordre( AVANCE,  0);
					break;
				default:
					break;
			}
			pong=L_NULL;
			boite_virtuelle();
			sortie_gagnant();
			break;

		case ALPHABET:

				lettre_M();
				decalage_interlettre();
				lettre_O();
				decalage_interlettre();
				lettre_N();
				decalage_interlettre();
				lettre_D();
				decalage_interlettre();
				lettre_A();
				decalage_interlettre();
				lettre_D();
				decalage_interlettre();
				lettre_A();

				   break;

		case BILLARD_INIT:
				   // do something in the stop state

					//initialization of variables
					//activate threads

					led1 = 0;
					led3 = 0;
					led5 = 0;
					led7 = 1;

					palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
					break;

		case BILLARD:
				   // do something in the stop state

				   break;

		case ENDGAME:
				   // do something in the stop state

					*currentState = MENU_PRINCIPAL;
					break;

	}

}

void state_compare(etats changeState){
	switch(currentStateManagement){


		case MENU_PRINCIPAL:
			switch(changeState){
					case MENU_PRINCIPAL:
						break;
					case PONG_INIT:
						nextState = changeState;
						break;
					case ALPHABET:
						nextState = changeState;
						break;
					case BILLARD_INIT:
						nextState = changeState;
						break;
					case ENDGAME:
						nextState = changeState;
						break;
					default:
						break;

			}
			break;

		case PONG_INIT:
			break;

		case PONG:
			switch(changeState){
				case ENDGAME:
					nextState = changeState;
					break;
				default:
					break;
			}
			break;

		case ALPHABET:
			switch(changeState){
				case ENDGAME:
					nextState = changeState;

					break;
				default:
					break;
			}
			break;

		case BILLARD_INIT:
			break;

		case BILLARD:
			switch(changeState){
				case ENDGAME:
					nextState = changeState;
					break;
				default:
					break;
			}
			break;

		case ENDGAME:
			nextState = changeState;

			break;
		default:
			break;
	}

	//compare currentState and changeState and decide the update of nextState
	if(currentStateManagement == MENU_PRINCIPAL && changeState == PONG_INIT)
	nextState = changeState;

}

void update_map_position(void)
{
	float d_x = 0;
	float d_y = 0;
	float dist = 0;


	float left_steps = 0;
	float right_steps = 0;

	float total_current_steps=0;
	float steps_difference=0;

	left_steps= (float)left_motor_get_pos();
	right_steps= (float)right_motor_get_pos();

	total_current_steps = (left_steps+right_steps)/DEUX;
	steps_difference=(total_current_steps-counted_current_steps);

	counted_current_steps=total_current_steps;

	dist =steps_to_cm(steps_difference);



	d_x = dist * (float)arm_cos_f32(ePuck.angle);
	d_y = dist * (float)arm_sin_f32(ePuck.angle);

	chprintf((BaseSequentialStream *)&SD3,"map_position  dist=%f !!! d_x=%f !!! d_y=%f Steeeeps=%f \n", dist, d_x, d_y, steps_difference);


	ePuck.x += d_x;
	ePuck.y += d_y;

}


void moteurs_tourne(float angle_rotation )
{
	float v_rot = 0;

	if (angle_rotation>0)
		v_rot = VITESSE_ROT;
	else v_rot = -VITESSE_ROT;

	float p_rotation = (PERIMETER_EPUCK * angle_rotation / (2*PI));


	motors_set_position(p_rotation, p_rotation, -v_rot, v_rot);

	//motors_stop_pos();


	update_map_angle(angle_rotation);



}

void update_map_angle(float angle_rotation)
{
	ePuck.angle += angle_rotation;

	if(ePuck.angle > PI)
		ePuck.angle -= ( 2*PI );

	if(ePuck.angle < (-PI))
		ePuck.angle += ( 2*PI );

	angle_rotation = 0;
}


void nouvel_ordre(order next_order, float angle_rotation){

	order previous_order = action;




	switch(previous_order){
			case ARRET:

				switch(next_order){
					case AVANCE:
						if(right_motor_get_pos() || left_motor_get_pos()){
							chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n previous=%d , next=%d\n", previous_order, next_order);
							motors_stop_pos();
						}
						action=AVANCE;
						moteurs_avance();

						break;

					case TOURNE:
						action = TOURNE;
						if(right_motor_get_pos() || left_motor_get_pos()){
							chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n previous=%d , next=%d\n", previous_order, next_order);
						}
						moteurs_tourne(angle_rotation);
						action = ARRET;
						break;

					case ARRET:
						if(right_motor_get_pos() || left_motor_get_pos()){
							chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n previous=%d , next=%d\n", previous_order, next_order);
						}

						action = next_order;
						motors_stop_pos();


						break;


					default:
						chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", previous_order, next_order);

				}

				break;

			case AVANCE:

				motors_stop_speed();

				switch(next_order){

					case AVANCE:


						action=next_order;

						postAvance_init();

						moteurs_avance();


						break;

					case TOURNE:

						motors_stop_speed();

						action=next_order;

						postAvance_init();

						moteurs_tourne(angle_rotation);
						action = ARRET;
						break;

					case ARRET:

						motors_stop_speed();
						action=next_order;

						postAvance_init();

						break;


					default:
						chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", previous_order, next_order);

				}
				break;

			case TOURNE:
				switch(next_order){
					case AVANCE:

						break;

					case TOURNE:

						break;

					case ARRET:

						break;


					default:
						chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", previous_order, next_order);

				}
				break;

				default:
					chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", previous_order, next_order);

		}
}

void postAvance_init(void){

	//chSysLock();
	update_map_position();
	counted_current_steps = ZERO;
	motors_stop_pos();
	//chSysUnlock();

}

void boite_virtuelle(void){
	if(ePuck.y>30){
		if(ePuck.angle>0 && ePuck.angle<=PI/2){
			nouvel_ordre(TOURNE, -2*ePuck.angle);
			nouvel_ordre(AVANCE, 0);
		}else if(ePuck.angle>PI/2 && ePuck.angle<PI){
			nouvel_ordre(TOURNE, 2*(PI-ePuck.angle));
			nouvel_ordre(AVANCE, 0);
		}
	}

	if(ePuck.y<-30){
		if(ePuck.angle<0 && ePuck.angle>=-PI/2){
			nouvel_ordre(TOURNE, -2*ePuck.angle);
			nouvel_ordre(AVANCE, 0);
		}else if(ePuck.angle<-PI/2 && ePuck.angle>-PI){
			nouvel_ordre(TOURNE, 2*((-PI)-ePuck.angle));
			nouvel_ordre(AVANCE, 0);
		}
	}
}

void sortie_gagnant(void){
	if((ePuck.x<-60)||(ePuck.x>60)){
		uint8_t point_temp_avant = point_joueur_avant, point_temp_arriere = point_joueur_arriere;
		if (ePuck.x<-60){
			point_joueur_avant++;
		}
		if (ePuck.x>60){
			point_joueur_arriere++;
		}
		go_home();
		if (point_temp_avant != point_joueur_avant || point_temp_arriere != point_joueur_arriere){
			for (int i = 0; i < point_joueur_avant; i++){
				set_led(LED3, 1);
				chThdSleepMilliseconds(500);
				set_led(LED3, 0);
				chThdSleepMilliseconds(500);
			}
			for (int j = 0; j < point_joueur_arriere; j++){
				set_led(LED7, 1);
				chThdSleepMilliseconds(500);
				set_led(LED7, 0);
				chThdSleepMilliseconds(500);
			}
		}
	}
}

void go_home(void){

	chprintf((BaseSequentialStream *)&SD3, "go home\n");
	//back_home = TRUE;
	//game_status = FALSE;
	//left_motor_set_speed(0);
	//right_motor_set_speed(0);


	float angle_rotation;

	//vitesse = cm_to_steps(VITESSE_MAX);

	//if (left_motor_get_pos() || right_motor_get_pos())
	//{
	//	update_map_position();
	//	left_motor_set_pos(0);
	//	right_motor_set_pos(0);
	//}

	motors_stop_speed();


	float coord_x=ePuck.x;
	float coord_y=ePuck.y;


	int j = 1;

	if(coord_y > 0 && coord_x > 0)
			j = 1;
	if(coord_y > 0 && coord_x < 0)
			j = 2;
	if(coord_y < 0 && coord_x < 0)
			j = 3;
	if(coord_y < 0 && coord_x > 0)
			j = 4;

	uint8_t num_quadrant = j;





		float norme = sqrt(coord_x*coord_x+coord_y*coord_y);

		if(norme == 0){
			chprintf((BaseSequentialStream *)&SD3,"NORME =0 IL DEVRAIT RIEN FAIRE PR GO HOME \n");
			return;
		}

		switch(num_quadrant)
		{
		case 1:
		angle_rotation = acos(coord_x/norme)-PI;
		break;
		case 2:
		angle_rotation = acos(coord_x/norme)-PI;
		break;
		case 3:
		angle_rotation = -acos(coord_x/norme)+PI;
		break;
		case 4:
		angle_rotation = -acos(coord_x/norme)+PI;
		break;
		}

		angle_rotation -= ePuck.angle;

		while(angle_rotation > PI)
				angle_rotation -= (2*PI);
		while(angle_rotation < (-PI))
				angle_rotation += (2*PI);


	if (norme<0.1 && ePuck.angle==0)
		return;

	nouvel_ordre(TOURNE, angle_rotation);
	nouvel_ordre(AVANCE, 0);

	norme = cm_to_steps(norme);



	return;

}

void wait_image_needed(void){
	chBSemWait(&image_needed_sem);
}

