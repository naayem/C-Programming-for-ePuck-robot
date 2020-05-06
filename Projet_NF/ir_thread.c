#include "ir_thread.h"

#define	FIRST_IR_SENSOR			0
#define	SECOND_IR_SENSOR		1
#define	SEVENTH_IR_SENSOR		6
#define EIGTH_IR_SENSOR			7
#define REFLET_FEUILLE			50

_Bool aide_detection_ligne(){
	if (get_calibrated_prox(FIRST_IR_SENSOR)>=REFLET_FEUILLE){
		return 1;
	}
	if (get_calibrated_prox(EIGTH_IR_SENSOR)>=REFLET_FEUILLE){
		return 1;
	}
	return 0;
}

_Bool obstacle_droite(int reflet_obstacle){
	if ((get_calibrated_prox(FIRST_IR_SENSOR)>=reflet_obstacle)||(get_calibrated_prox(SECOND_IR_SENSOR)>=reflet_obstacle)){
		return 1;
	}
	return 0;
}

_Bool obstacle_gauche(int reflet_obstacle){
	if ((get_calibrated_prox(EIGTH_IR_SENSOR)>=reflet_obstacle)||(get_calibrated_prox(SEVENTH_IR_SENSOR)>=reflet_obstacle)){
		return 1;
	}
	return 0;
}
