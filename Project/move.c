#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <main.h>
#include <motors.h>
#include <process_image.h>
#include <colors.h>
#include <move.h>
#include <follow_line.h>
#include <run_over.h>

void move(void)
{
	chprintf((BaseSequentialStream *)&SD3,"red\n");
	if(return_obstacle())
	{
		right_motor_set_speed(return_speed_r_ro());
		left_motor_set_speed(return_speed_l_ro());
	}
	else if(return_color_detected())
	{
		right_motor_set_speed(return_speed_r_c());
		left_motor_set_speed(return_speed_l_c());
	}
	else
	{
		right_motor_set_speed(return_speed_r_fl());
		left_motor_set_speed(return_speed_l_fl());
	}
}
