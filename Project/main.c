#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>//////////////////////////////////////////////
#include <camera/po8030.h>
#include <chprintf.h> ///////////////////////////A enlever////////////////////////////
#include <sensors/proximity.h>

#include <process_image.h>
#include <follow_line.h>
#include <colors.h>
#include <run_over.h>
#include <move.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{/*
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
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

    //starts the serial communication////////////A enlever?//////////
    serial_start();
    //start the USB communication////////////A enlever?//////////
    usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	//Thread for the IR distance sensor
	proximity_start();
	calibrate_ir();

	//stars the threads for the pi regulator and the processing of the image
	process_image_start();
	line_follow_start();
	skirt_start();
	color_detection_start();

    /* Infinite loop. */
    while (1)
    {
    	move();
    	//waits 1 second
    	chThdYield();
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
