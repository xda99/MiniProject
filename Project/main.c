/**
 * @file    main.c
 * @brief   Initializes the sensors and the motors and starts the threads
 *
 * @author  David Niederhauser and Valentin Roch
 */

#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <sensors/proximity.h>
#include <audio/audio_thread.h>
#include <process_image.h>
#include <follow_line.h>
#include <colors.h>
#include <run_over.h>
#include <move.h>

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
	//HAL initialization
    halInit();
    // Initializes the kernel
    chSysInit();
    //This function enables the MPU
    mpu_init();
    //Starts the serial communication
    serial_start();
	//Starts the DAC module. Power of the audio amplifier and DAC peripheral
	dac_start();

    //Starts the camera
    dcmi_start();
	po8030_start();
	//Starts the motors
	motors_init();
	//Starts the IR distance sensors
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	proximity_start();
	calibrate_ir();

	//Starts the thread for the processing of the image
	process_image_start();

	//Starts the line following thread
	line_follow_start();

	//Starts the obstacle avoidance thread
	skirt_start();

	//Starts the color detection thread
	color_detection_start();

    while (1)
    {
    	//The only function that set a speed to the motors
    	move();
    	chThdYield();
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
