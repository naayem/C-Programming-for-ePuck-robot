#include <main.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>

#include <motors.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>


#include<game_management.h>
#include <camera_processing.h>
#include <sensors/proximity.h>


messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);


static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();
    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();
    //starts the camera
      dcmi_start();
  	po8030_start();
    //inits the motors
    motors_init();
    //starts the microphones
    mic_start(&processAudioData);
    //Enables data transfer between threads
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    chThdSleepMilliseconds(500);

    //starts the mapping thread
    mapping_start();
 	//stars the threads for the processing of the image
 	process_image_start();
 	//initializes IR sensors
 	proximity_start();

    /* Infinite loop. */
    while (1)
    {
    	management();
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
