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
