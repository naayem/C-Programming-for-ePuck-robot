#include "ir_thread.h"


#define OBSTACLE_DISTANCE	300
#define	FIRST_IR			0
#define THIRD_IR			2
#define SEVENTH_IR			5
#define EIGTH_IR			7

_Bool obstacle(void){
	for (int i=FIRST_IR; i<THIRD_IR; i++){
		if (get_prox(i)>=OBSTACLE_DISTANCE){
			return 1;
		}
	}
	for (int j=SEVENTH_IR; j<EIGTH_IR+1; j++){
		if (get_prox(j)>=OBSTACLE_DISTANCE){
			return 1;
		}
	}
	return 0;
}
