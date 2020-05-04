#include <chprintf.h>

#include "rebond.h"
#include "sensors/imu.h"



static THD_WORKING_AREA(waThdAccTest, 1028);
static THD_FUNCTION(ThdAccTest, arg) {


    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    volatile systime_t time;

    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
           imu_msg_t imu_values;


    //wait 2 sec to be sure the e-puck is in a stable position
           //chThdSleepMilliseconds(2000);
           calibrate_acc();

    while(1){
    	time = chVTGetSystemTime();

	//wait for new measures to be published
	messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));


	float angle = (atan2(imu_values.acceleration[X_AXIS], imu_values.acceleration[Y_AXIS]))*180/PI;
	float norme = sqrt(imu_values.acceleration[X_AXIS]*imu_values.acceleration[X_AXIS]+ imu_values.acceleration[Y_AXIS]*imu_values.acceleration[Y_AXIS]);


	chprintf((BaseSequentialStream *)&SD3, "%Ax=%.2f Ay=%.2f Az=%.2f Gx=%.2f Gy=%.2f Gz=%.2f (%x)\r\n\n",
			norme, norme, imu_values.acceleration[Z_AXIS],
					imu_values.gyro_rate[X_AXIS], imu_values.gyro_rate[Y_AXIS], imu_values.gyro_rate[Z_AXIS],
					imu_values.status);



	//chThdSleepMilliseconds(100);
	chThdSleepUntilWindowed(time, time + MS2ST(10));
    	}

}


void rebond_start(void){
	chThdCreateStatic(waThdAccTest, sizeof(waThdAccTest), NORMALPRIO, ThdAccTest, NULL);
}

