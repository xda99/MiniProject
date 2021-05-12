#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>
#include <colors.h>
#include <follow_line.h>
#include <move.h>
#include <run_over.h>

static THD_WORKING_AREA(waMove, 1024);
static THD_FUNCTION(Move, arg)
{
    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1)
    {
        if(return_obstacle())
        {
        	chprintf((BaseSequentialStream *)&SD3,"1\n");
        	right_motor_set_speed(return_speed_r_ro());
        	left_motor_set_speed(return_speed_l_ro());
        }
       /* else if(return_color())
        {
        	right_motor_set_speed();
        	left_motor_set_speed();
        }*/
        else
        {
        	chprintf((BaseSequentialStream *)&SD3,"4\n");
        	right_motor_set_speed(return_speed_r_fl());
        	left_motor_set_speed(return_speed_l_fl());
        }
		chThdSleepMilliseconds(10);
    }
}

void move_start(void)
{
	chThdCreateStatic(waMove, sizeof(waMove), NORMALPRIO, Move, NULL);
}
