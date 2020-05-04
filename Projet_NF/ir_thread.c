#include "ir_thread.h"

#define	FIRST_IR			0
#define THIRD_IR			2
#define SEVENTH_IR			5
#define EIGTH_IR			7

_Bool obstacle(int obstacle_distance){
	for (int i=FIRST_IR; i<THIRD_IR; i++){
		if (get_prox(i)>=obstacle_distance){
			return 1;
		}
	}
	for (int j=SEVENTH_IR; j<EIGTH_IR+1; j++){
		if (get_prox(j)>=obstacle_distance){
			return 1;
		}
	}
	return 0;
}
