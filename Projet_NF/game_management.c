#include <main.h>
#include <game_management.h>
#include <math.h>
#include <arm_math.h>
#include <audio_processing.h>
#include <ir_thread.h>
#include <motors_processing.h>
#include <camera_processing.h>
#include <leds.h>
#include <letter_writing.h>

#define TAILLE_BOITE_PONG_X		30
#define TAILLE_BOITE_PONG_Y		20
#define ROTATION_180_DEGRE		180*DEG_TO_RAD
#define ROTATION_135_DEGRE		135*DEG_TO_RAD
#define QUADRANT_SUP_DROIT		1
#define QUADRANT_SUP_GAUCHE		2
#define QUADRANT_INF_GAUCHE		3
#define QUADRANT_INF_DROIT		4
#define POINTS_POUR_GAGNER		3

static mapping ePuck = {0,0,0};									//Position du ePuck en mode Pong
static etats nextState = MENU_PRINCIPAL;						//Prochain etat a evaluer pour mettre a jour la machine d'etat principal
static etats currentStateManagement = MENU_PRINCIPAL;			//Etat de la machine d'etat principal
static order action = ARRET;									//Etat de la sous-machine d'etat des mouvement du robot
static lettre lettre_state = AUCUN;								//Etat de la sous-machine d'etat d'ecriture de lettres
static float counted_current_steps = 0;							//Compteur des pas moteurs comptabilises dans les fonction de mis a jour de position
static uint8_t point_joueur_avant = 0, point_joueur_arriere = 0;//Compteur de points pour le jeu Pong
static uint8_t case_init = 1;									//Variable pour l'initialisation des led afin de modifier leur etat qu'une fois

/***************************INTERNAL FUNCTIONS************************************/

/**
 * @brief Update la position de mapping du ePuck selon les pas effectuÃ©s par les moteurs
 */
void update_map_position(void)
{
	float left_steps = (float)left_motor_get_pos();
	float right_steps = (float)right_motor_get_pos();

	float moyenne_current_steps = (left_steps+right_steps)/2;
	float steps_difference= (moyenne_current_steps-counted_current_steps);

	counted_current_steps = moyenne_current_steps;

	float dist = steps_to_cm(steps_difference);

	ePuck.x += dist * (float)arm_cos_f32(ePuck.angle);
	ePuck.y += dist * (float)arm_sin_f32(ePuck.angle);
}

/**
 * @brief update l'angle de mapping du ePuck selon l'angle de rotation qu'on lui a demandÃ© d'effectuer
 *
 * @param angle_rotation			Angle de rotation a effectué par l'ePuck
 *
 */
void update_map_angle(float angle_rotation)
{
	ePuck.angle += angle_rotation;

	while(ePuck.angle > PI){
		ePuck.angle -= ( 2*PI );
		chThdSleepMilliseconds(2);
	}

	while(ePuck.angle <= (-PI)){
		ePuck.angle += ( 2*PI );
		chThdSleepMilliseconds(2);
	}
}

/**
 * @brief Effectue des Ã©tapes nÃ©cessaire au mapping de l'ePuck apres l'interruption d'une trajectoire droite.
 */
void postAvance_init(void){
	chSysLock();
	update_map_position();
	counted_current_steps = 0;
	chSysUnlock();
	motors_stop_pos();
}

/**
 * @brief Execute un des trois ordres possibles qui sont ARRET, TOURNE et AVANCE. Gere l'execution de l'ordre et le mapping en fonction.
 *
 * @param next_order			Ordres de dÃ©placement.
 * @param angle_rotation	 	Angle de rotation a effectuer, evaluÃ© cas de rotation TOURNE. Non evaluÃ© en mode ARRET ou AVANCE.
 *
 */
void nouvel_ordre(order next_order, float angle_rotation){
	switch(action){
		case ARRET:
			switch(next_order){
				case AVANCE:
					if(right_motor_get_pos() || left_motor_get_pos()){
						chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n previous=%d , next=%d\n", action, next_order);
						motors_stop_pos();
					}
					action=AVANCE;
					moteurs_avance();
					break;

				case TOURNE:
					if(right_motor_get_pos() || left_motor_get_pos()){
						chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n previous=%d , next=%d\n", action, next_order);
					}
					action = TOURNE;
					moteurs_tourne(angle_rotation);
					action = ARRET;
					break;

				case ARRET:
					if(right_motor_get_pos() || left_motor_get_pos()){
						chprintf((BaseSequentialStream *)&SD3,"error arret positon non nulle\n previous=%d , next=%d\n", action, next_order);
					}
					action = ARRET;
					motors_stop_pos();
					break;

				default:
					chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", action, next_order);
					break;
			}
			break;

		case AVANCE:
			motors_stop_speed();
			postAvance_init();
			switch(next_order){
				case AVANCE:
					action=AVANCE;
					moteurs_avance();
					break;

				case TOURNE:
					action=TOURNE;
					moteurs_tourne(angle_rotation);
					action = ARRET;
					break;

				case ARRET:
					action=ARRET;
					break;

				default:
					chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", action, next_order);
					break;
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
					chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", action, next_order);
					break;
			}
			break;

		default:
			chprintf((BaseSequentialStream *)&SD3,"ERROR operators not coorect \n!!! previous=%d , next=%d\n", action, next_order);
			break;
	}
}

/**
 * @brief Gestion des leds 1, 3, 5, 7 de l'ePuck. 0 pour une led eteinte et 1 pour une led allume.
 *
 * @param led1			Etat de la led1 a mettre a jour.
 * @param led3			Etat de la led3 a mettre a jour.
 * @param led5			Etat de la led5 a mettre a jour.
 * @param led7			Etat de la led7 a mettre a jour.
 *
 */
void leds_management(uint8_t led1,uint8_t led3,uint8_t led5 ,uint8_t led7){
	palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
	palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
	palWritePad(GPIOD, GPIOD_LED5, led5 ? 0 : 1);
	palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
}

/**
 * @brief Envoi l'ePuck en direction de sa position initial en mode PONG et envoi l'ePuck a sa position initial en cas de victoire.
 *
 */
void go_home(void){

	float angle_rotation=0, coord_x=ePuck.x, coord_y=ePuck.y;
	uint8_t num_quadrant;

	motors_stop_speed();

	float norme = sqrt(coord_x*coord_x+coord_y*coord_y);

	if(norme == 0){
		chprintf((BaseSequentialStream *)&SD3,"NORME =0 IL DEVRAIT RIEN FAIRE PR GO HOME \n");
		return;
	}

	if (norme<ERROR_THRESHOLD && ePuck.angle==0)
		return;

	if(coord_y >= 0 && coord_x > 0)
		num_quadrant = QUADRANT_SUP_DROIT;
	if(coord_y > 0 && coord_x <= 0)
		num_quadrant = QUADRANT_SUP_GAUCHE;
	if(coord_y <= 0 && coord_x < 0)
		num_quadrant = QUADRANT_INF_GAUCHE;
	if(coord_y < 0 && coord_x >= 0)
		num_quadrant = QUADRANT_INF_DROIT;

	switch(num_quadrant){
		case QUADRANT_SUP_DROIT:
			angle_rotation = acos(coord_x/norme)-PI;
			break;
		case QUADRANT_SUP_GAUCHE:
			angle_rotation = acos(coord_x/norme)-PI;
			break;
		case QUADRANT_INF_GAUCHE:
			angle_rotation = -acos(coord_x/norme)+PI;
			break;
		case QUADRANT_INF_DROIT:
			angle_rotation = -acos(coord_x/norme)+PI;
			break;
		default:
			break;
	}

	angle_rotation -= ePuck.angle;

	while(angle_rotation > PI){
		angle_rotation -= (2*PI);
		chThdSleepMilliseconds(1);
	}
	while(angle_rotation <= (-PI)){
		angle_rotation += (2*PI);
		chThdSleepMilliseconds(1);
	}

	nouvel_ordre(TOURNE, angle_rotation);
	nouvel_ordre(AVANCE, 0);
	chThdSleepMilliseconds(100);


	if ((point_joueur_avant >= POINTS_POUR_GAGNER) || (point_joueur_arriere >= POINTS_POUR_GAGNER)){
		motors_stop_speed();
		motors_set_position(norme, norme, VITESSE_GENERALE, VITESSE_GENERALE);	// retourne a la position inital a une distance norme cm
																				// avec l'etat action AVANCE toujours active
		nouvel_ordre(TOURNE, -ePuck.angle);
	}
}


/**
 * @brief Fixe les contours d'un terrain virtuelle et decide les interactions avec ces contours
 */
void boite_virtuelle(void){
	if(ePuck.y>TAILLE_BOITE_PONG_Y){
		if(ePuck.angle>0 && ePuck.angle<=PI/2){
			nouvel_ordre(TOURNE, -2*ePuck.angle);
			nouvel_ordre(AVANCE, 0);
		}else if(ePuck.angle>PI/2 && ePuck.angle<=PI){
			nouvel_ordre(TOURNE, 2*(PI-ePuck.angle));
			nouvel_ordre(AVANCE, 0);
		}
	}

	if(ePuck.y<-TAILLE_BOITE_PONG_Y){
		if(ePuck.angle<0 && ePuck.angle>=-PI/2){
			nouvel_ordre(TOURNE, -2*ePuck.angle);
			nouvel_ordre(AVANCE, 0);
		}else if(ePuck.angle<-PI/2 && ePuck.angle>-PI){
			nouvel_ordre(TOURNE, 2*((-PI)-ePuck.angle));
			nouvel_ordre(AVANCE, 0);
		}
	}
}

/*en cas de dépassement d'un côté d'un joueur
 * 1 point pour l'autre et retour dans la direction initiale
 */
void sortie_gagnant(void){
	if((ePuck.x<(-TAILLE_BOITE_PONG_X))||(ePuck.x>TAILLE_BOITE_PONG_X)){
		//ajoutez point à celui qui marque
		if (ePuck.x<=(-TAILLE_BOITE_PONG_X)){
			point_joueur_avant++;
		}
		if (ePuck.x>=TAILLE_BOITE_PONG_X){
			point_joueur_arriere++;
		}

		//si un des joueurs possèdent 3 points la partie se termine
		if (point_joueur_avant >= POINTS_POUR_GAGNER){
			set_led(LED3, 1);
			set_body_led(1);
			go_home();
			nextState = ENDGAME;
			return;
		}
		if (point_joueur_arriere >= POINTS_POUR_GAGNER){
			set_led(LED7, 1);
			set_body_led(1);
			go_home();
			nextState = ENDGAME;
			return;
		}

		//sinon repartir vers le point initial
		go_home();

		//en affichant les points de chaque joueur
		//Remarque: Zone grise aux extremites en x ou tant qu'il compte les points
		//l'epuck ignore ne reagit pas tant qu'il compte les points.
		for (int i = 0; i < point_joueur_avant; i++){
			set_led(LED3, 1);
			chThdSleepMilliseconds(100);
			set_led(LED3, 0);
			chThdSleepMilliseconds(100);
		}
		for (int j = 0; j < point_joueur_arriere; j++){
			set_led(LED7, 1);
			chThdSleepMilliseconds(100);
			set_led(LED7, 0);
			chThdSleepMilliseconds(100);
		}
	}
}

/*
 * Thread pour l'update de la position a une frequence maximale de 100 Hz en etat AVANCE.
 * Envoie la position de l'ePuck a l'ordinateur.
 */
static THD_WORKING_AREA(waThdMapping, 384);
static THD_FUNCTION(ThdMapping, arg) {
    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    volatile systime_t time;
    while(1){
		time = chVTGetSystemTime();
		if(action==AVANCE){
			update_map_position();
		}
		chThdSleepUntilWindowed(time, time + MS2ST(10));
		chprintf((BaseSequentialStream *)&SD3,"update x=%f .... y=%f .... angle=%f  \n", ePuck.x, ePuck.y, ePuck.angle);
    }
}

/*************************END INTERNAL FUNCTIONS**********************************/


/****************************PUBLIC FUNCTIONS*************************************/

void management(){
	// Switch de la machine d'etat principal
	switch (currentStateManagement) {
	   case MENU_PRINCIPAL:
		   if (case_init){
			   leds_management(1,0,0,0);
			   case_init=0;
		   }
		   currentStateManagement = nextState;
		   break;

		case PONG_INIT: //Lecture unique du case lorsqu'on rentre en mode PONG
			calibrate_ir();
			leds_management(1,0,1,0);
			nouvel_ordre(AVANCE, 0);
			case_init = 1;
			currentStateManagement = PONG;
			nextState = PONG;
			break;

		case PONG:
			if (case_init){
			   leds_management(0,0,1,0);
			   case_init=0;
			}

			sortie_gagnant();
			boite_virtuelle();

			switch(close_line()){
				case L_DROITE:
					nouvel_ordre(TOURNE, ROTATION_135_DEGRE);
					nouvel_ordre(AVANCE, 0);
					break;
				case L_GAUCHE:
					nouvel_ordre(TOURNE, -ROTATION_135_DEGRE);
					nouvel_ordre(AVANCE, 0);
					break;
				default:
					break;
			}

			if (obstacle_demi_tour()){
				nouvel_ordre(TOURNE, ROTATION_180_DEGRE);
				nouvel_ordre(AVANCE, 0);
			}
			currentStateManagement = nextState;
			break;

		case ALPHABET:
			if (lettre_state == AUCUN){
			   leds_management(1,1,0,1);
			   lettre_state = ALPHA_INIT;
			}

			if (letter_ready()){
				lettre_state = get_letter_state();
				switch (lettre_state){
					case LETTRE_M:
						lettre_M();
						decalage_interlettre();
						break;
					case LETTRE_O:
						lettre_O();
						decalage_interlettre();
						break;
					case LETTRE_N:
						lettre_N();
						decalage_interlettre();
						break;
					case LETTRE_D:
						lettre_D();
						decalage_interlettre();
						break;
					case LETTRE_A:
						lettre_A();
						decalage_interlettre();
						break;
					case ELIOT:
						ecriture_eliot();
						break;
					default:
						break;
				}
				next_letter();
			}
			currentStateManagement = nextState;
			break;

		case ENDGAME: //fin des modes Pong et Alphabet une seule lecture pour réinitialiser les variables
			nouvel_ordre(ARRET, 0);
			ePuck.x = 0;
			ePuck.y = 0;
			ePuck.angle = 0;
			point_joueur_arriere=0;
			point_joueur_avant=0;
			set_front_led(0);
			set_body_led(0);
			leds_management(0,0,0,0);
			lettre_state = AUCUN;
			case_init = 1;

			nextState = MENU_PRINCIPAL;
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
				case PONG_INIT:
					nextState = changeState;
					break;
				case ALPHABET:
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

		default:
			break;
	}
}
