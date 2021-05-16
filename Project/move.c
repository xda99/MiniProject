/**
 * @file    move.c
 * @brief	Receives from the files "follow_line", "colors" and "run_over" the speed to give to both motors
 * 			and sets it
 *
 *
 * @author  David Niederhauser and Valentin Roch
 */
#include <move.h>
#include <main.h>
#include <motors.h>
#include <run_over.h>
#include <colors.h>
#include <follow_line.h>

void move(void)
{
	if(return_obstacle())
	{
		right_motor_set_speed(return_speed_r_ro());
		left_motor_set_speed(return_speed_l_ro());
	}
	else
	{
		right_motor_set_speed(return_speed_r_fl()-return_speed_reduction());
		left_motor_set_speed(return_speed_l_fl()-return_speed_reduction());
	}
}
