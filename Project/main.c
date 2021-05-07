#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>
#include <sensors/proximity.h>

#include <pi_regulator.h>
#include <process_image.h>
#include <follow_line.h>
#include <colors.h>
#include <run_over.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void angle_rotation(float angle, int16_t speed_r)
{
	int16_t right_init=right_motor_get_pos();
	int16_t left_init=left_motor_get_pos();
	do
	{
		left_motor_set_speed(speed_r);
		right_motor_set_speed(-speed_r);
	}while(((float)abs(right_init-right_motor_get_pos())*0.13f <(53/2)*angle) && ((float)abs(left_init-left_motor_get_pos())*0.13f <53*angle/2));

	left_motor_set_speed(0);
	right_motor_set_speed(0);
}




void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	/*chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);*/

}

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
    //start the USB communication
    usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	//Thread for the IR distance sensor
	proximity_start();
	chThdSleepMilliseconds(1000);
	calibrate_ir();

	//stars the threads for the pi regulator and the processing of the image
	process_image_start();
//	line_follow_start();
//	color_detection_start();
//	skirt_start();

    /* Infinite loop. */
    while (1) {

    	angle_rotation(1.570796,-SPEED_EPUCK);

    	/*chprintf((BaseSequentialStream *)&SD3,"Prox0=%d\n",get_calibrated_prox(0));
    	chprintf((BaseSequentialStream *)&SD3,"Prox1=%d\n",get_calibrated_prox(1));
    	chprintf((BaseSequentialStream *)&SD3,"Prox2=%d\n",get_calibrated_prox(2));
    	chprintf((BaseSequentialStream *)&SD3,"Prox3=%d\n",get_calibrated_prox(3));
    	chprintf((BaseSequentialStream *)&SD3,"Prox4=%d\n",get_calibrated_prox(4));
    	chprintf((BaseSequentialStream *)&SD3,"Prox5=%d\n",get_calibrated_prox(5));
    	chprintf((BaseSequentialStream *)&SD3,"Prox6=%d\n",get_calibrated_prox(6));
    	chprintf((BaseSequentialStream *)&SD3,"Prox7=%d\n",get_calibrated_prox(7));
//    	chprintf((BaseSequentialStream *)&SD3,"Prox= %d\n",get_prox(0));*/
    	//waits 1 second
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
