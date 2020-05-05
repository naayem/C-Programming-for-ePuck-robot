#include <main.h>
#include <game_management.h>

#include <math.h>
#include <arm_math.h>

#include <audio_processing.h>
#include <ir_thread.h>
#include <motors_processing.h>
#include <camera_processing.h>


static BSEMAPHORE_DECL(image_needed_sem, TRUE);


//Faire comme dans motor.c???
static mapping ePuck = {0,0,0};
static etats nextState = MENU_PRINCIPAL;
static etats currentStateManagement = MENU_PRINCIPAL;
static order action = ARRET;
static float counted_current_steps = 0;

/***************************INTERNAL FUNCTIONS************************************/

/**
 * @brief Update la position de mapping du ePuck selon les pas effectués par les moteurs
 *
 */
void update_map_position(void)
{
	float left_steps= (float)left_motor_get_pos();
	float right_steps= (float)right_motor_get_pos();

	float total_current_steps= (left_steps+right_steps)/DEUX;
	float steps_difference= (total_current_steps-counted_current_steps);

	counted_current_steps=total_current_steps;

	float dist =steps_to_cm(steps_difference);

	ePuck.x += dist * (float)arm_cos_f32(ePuck.angle);
	ePuck.y += dist * (float)arm_sin_f32(ePuck.angle);

	chprintf((BaseSequentialStream *)&SD3,"map_position  dist=%f !!! d_x=%f !!! d_y=%f Steeeeps=%f \n", dist, ePuck.x, ePuck.y, steps_difference);
}

/**
 * @brief update l'angle de mapping du ePuck selon l'angle de rotation qu'on lui a demandé d'effectuer
 *
 * @param angle_rotation			Angle de rotation a effectué par l'ePuck
 *
 */
void update_map_angle(float angle_rotation)
{
	ePuck.angle += angle_rotation;

	if(ePuck.angle > PI)
		ePuck.angle -= ( 2*PI );

	if(ePuck.angle < (-PI))
		ePuck.angle += ( 2*PI );

	angle_rotation = 0;
}

/**
 * @brief Effectue des étapes nécessaire au mapping de l'ePuck apres l'interruption d'une trajectoire droite.
 *
 */
void postAvance_init(void){
	//chSysLock();
	update_map_position();
	counted_current_steps = ZERO;
	motors_stop_pos();
	//chSysUnlock();
}


/**
 * @brief Execute un des trois ordres possibles qui sont ARRET, TOURNE et AVANCE. Gere l'execution de l'ordre et le mapping en fonction.
 *
 * @param next_order			Ordres de déplacement.
 * @param angle_rotation	 	Angle de rotation a effectuer, evalué cas de rotation TOURNE. Non evalué en mode ARRET ou AVANCE.
 *
 *
 */
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



void leds_management(uint8_t led1,uint8_t led3,uint8_t led5 ,uint8_t led7){

					palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
					palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
}


/**
 * @brief Envoi l'ePuck en direction de sa position initial en mode PONG et envoi l'ePuck a sa position initial en mode BILLARD et ALPHABET.
 *
 */
void go_home(void){

	float angle_rotation;
	float coord_x=ePuck.x;
	float coord_y=ePuck.y;

	motors_stop_speed();

	uint8_t j = 1;

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


	if (norme<ERROR_THRESHOLD && ePuck.angle==0)
		return;

	nouvel_ordre(TOURNE, angle_rotation);
	nouvel_ordre(AVANCE, 0);

	norme = cm_to_steps(norme);
	return;
}


/**
 * @brief Fixe les contours d'un terrain virtuelle et decide les interactions avec ces contours
 *
 */
void boite_virtuelle(void){
	if(ePuck.y>40){
		if(ePuck.angle>0 && ePuck.angle<=PI/2){
			nouvel_ordre(TOURNE, -2*ePuck.angle);
			nouvel_ordre(AVANCE, 0);
		}else if(ePuck.angle>PI/2 && ePuck.angle<PI){
			nouvel_ordre(TOURNE, 2*(PI-ePuck.angle));
			nouvel_ordre(AVANCE, 0);
		}else go_home();
	}

	if(ePuck.y<-40){
			if(ePuck.angle<0 && ePuck.angle>=-PI/2){
				nouvel_ordre(TOURNE, -2*ePuck.angle);
				nouvel_ordre(AVANCE, 0);
			}else if(ePuck.angle<-PI/2 && ePuck.angle>-PI){
				nouvel_ordre(TOURNE, 2*((-PI)-ePuck.angle));
				nouvel_ordre(AVANCE, 0);
			}else go_home();
	}
}


static THD_WORKING_AREA(waThdMapping, 628);
static THD_FUNCTION(ThdMapping, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    volatile systime_t time;
    while(1){
    			time = chVTGetSystemTime();
    			if(action==AVANCE){
    				update_map_position();
    			}
    			chprintf((BaseSequentialStream *)&SD3,"update x=%f .... y=%f .... angle=%f  \n", ePuck.x, ePuck.y, ePuck.angle);
			chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

/*************************END INTERNAL FUNCTIONS**********************************/


/****************************PUBLIC FUNCTIONS*************************************/

/*
static const uint8_t step_table[8][4] = {
    {1, 0, 1, 0},
	{0, 0, 1, 0},
    {0, 1, 1, 0},
	{0, 1, 0, 0},
    {0, 1, 0, 1},
	{0, 0, 0, 1},
    {1, 0, 0, 1},
	{1, 0, 0, 0},
};
*/
void leds_update(const uint8_t *out)  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
    /* left motor */
    out[0] ? palWritePad(GPIOD, GPIOD_LED1, 0) : palWritePad(GPIOD, GPIOD_LED1, 1);
    out[1] ? palWritePad(GPIOD, GPIOD_LED3, 0) : palWritePad(GPIOD, GPIOD_LED3, 1);
    out[2] ? palWritePad(GPIOD, GPIOD_LED5, 0) : palWritePad(GPIOD, GPIOD_LED5, 1);
    out[3] ? palWritePad(GPIOD, GPIOD_LED7, 0) : palWritePad(GPIOD, GPIOD_LED7, 1);

}

void management(){
	//we create variables for the led in order to turn them off at each loop and to
	//select which one to turn on
	posLine pong;

	switch (currentStateManagement) {
	   case MENU_PRINCIPAL:
		   // do something in the idle state
		    leds_management(1,0,0,0);
		    currentStateManagement = nextState;
		   break;

		case PONG_INIT:
		   // do something in the stop state
			//initialization of variables
			//activate threads
			proximity_start(); // peut etre a mettre dans le main!!!!!!!!!!
			calibrate_ir();
			leds_management(0,1,1,0);
			currentStateManagement = PONG;
		   break;

		case PONG:
				   // do something in the stop state
					chBSemSignal(&image_needed_sem);
					leds_management(0,1,1,1);

					if ( obstacle() ){
						nouvel_ordre( TOURNE,  90*DEG_TO_RAD);
						nouvel_ordre( AVANCE,  0);
					}
					pong = close_line();

					switch(pong){
						case L_NULL:
							break;
						case L_MILIEU:
							nouvel_ordre( TOURNE,  180*DEG_TO_RAD);
							nouvel_ordre( AVANCE,  0);
							break;
						case L_DROITE:
							nouvel_ordre( TOURNE,  90*DEG_TO_RAD);
							nouvel_ordre( AVANCE,  0);
							break;
						case L_GAUCHE:
							nouvel_ordre( TOURNE,  -90*DEG_TO_RAD);
							nouvel_ordre( AVANCE,  0);
							break;
						default:
							break;
					}
					pong=L_NULL;
					boite_virtuelle();
				    currentStateManagement = nextState;
					break;

		case ALPHABET:
				   // do something in the stop state
					leds_management(0,1,1,0);
					currentStateManagement = nextState;
				   break;

		case BILLARD_INIT:
				   // do something in the stop state
					//initialization of variables
					//activate threads
					leds_management(0,0,0,1);
					currentStateManagement = BILLARD;
					break;

		case BILLARD:
				   // do something in the stop state
					leds_management(0,0,1,1);
		    			currentStateManagement = nextState;
				   break;

		case ENDGAME:
				   // do something in the stop state
					currentStateManagement = MENU_PRINCIPAL;
					break;
	}
}



void mapping_start(void){
chThdCreateStatic(waThdMapping, sizeof(waThdMapping), NORMALPRIO, ThdMapping, NULL);
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
}



void wait_image_needed(void){
    chBSemWait(&image_needed_sem);
}








