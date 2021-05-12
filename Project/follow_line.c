#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <run_over.h>
#include <process_image.h>
#include <colors.h>

#define THRESHOLD_CURVE 				290 //number of pixels that define the beginning of a curve
#define ROTATION 						500 //for a 180 degrees rotation
#define TRESHOLD						10
#define WHEEL_RAYON						40//mm

static bool turn=false;
static bool right=false;
static bool begin_turn=false;
static bool position_done=false;
static uint16_t speed_virage_cor = 0;
static int16_t speed_r=0;
static int16_t speed_l=0;


//position_r and position_l in mm, speed in step/s
void position(float distance, int16_t speed)
{
	left_motor_set_pos(0);
	speed_l=speed;
	speed_r=speed;
	if(left_motor_get_pos()*0.13f>distance)
	{
		position_done=true;
	}
}

void curve(void)
{
	if(((get_line_position()-(IMAGE_BUFFER_SIZE/2))>0) && !turn)//Curve is on the right
	{
		right=true;
	}
	else if(((get_line_position()-(IMAGE_BUFFER_SIZE/2))<0) && !turn) //Curve is on the left
	{
		right=false;
	}
	if(!turn)
	{
		turn=true;
		right_motor_set_pos(0);
	}

	if((right_motor_get_pos()>CAMERA__DISTANCE_CORRECTION) && !begin_turn) //Peut commencer à tourner
	{
		begin_turn=true;
	}
	if(!begin_turn)
	{
		speed_r=SPEED_EPUCK;
		speed_l=SPEED_EPUCK;
	}

	if(right_motor_get_pos()==40)
	{
		speed_virage_cor=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2)); //Speed correction
	}

	if(right && turn && begin_turn)
	{
		speed_r=SPEED_EPUCK-2*speed_virage_cor-80;
		speed_l=SPEED_EPUCK+2*speed_virage_cor+80;
	}
	else if(turn && begin_turn && !right)
	{
		speed_r=SPEED_EPUCK+2*speed_virage_cor+80;
		speed_l=SPEED_EPUCK-2*speed_virage_cor-80;
	}

	if(get_line_not_found() == LINE_FOUND) //Ligne retrouvée
	{
		turn=false;
		begin_turn=false;
		position_done=false;
	}
}

void straight_line(int16_t speed_correction)
{
	if((get_line_not_found() != LINE_FOUND) && !position_done)
	{
		position(30, SPEED_EPUCK);
	}
	if(get_line_not_found() == LINE_FOUND)
	{
		speed_r=SPEED_EPUCK - ROTATION_COEFF * speed_correction;
		speed_l=SPEED_EPUCK + ROTATION_COEFF * speed_correction;
	}
}

static THD_WORKING_AREA(waLineFollow, 2048);
static THD_FUNCTION(LineFollow, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    //computes the speed to give to the motors
    int16_t speed = SPEED_EPUCK;
    int16_t speed_correction = 0;

    while(1)
    {
    	if(!return_obstacle())
    	{
			//computes a correction factor to let the robot rotate to be in front of the line
			speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

			//if the line is nearly in front of the camera, don't rotate
			if(((abs(speed_correction) < ROTATION_THRESHOLD) && !turn))
			{
				speed_correction = 0;
				speed_r=speed - ROTATION_COEFF * speed_correction;
				speed_l=speed + ROTATION_COEFF * speed_correction;
			}
			else if((get_line_width() > THRESHOLD_CURVE || turn)) //=>virage
			{
				curve();
			}
			else if(!turn)
			{
				straight_line(speed_correction);
			}
    	}
	    	chThdYield();
    }
}

int16_t return_speed_l_fl(void)
{
	return speed_l;
}

int16_t return_speed_r_fl(void)
{
	return speed_r;
}

void line_follow_start(void){
	chThdCreateStatic(waLineFollow, sizeof(waLineFollow), NORMALPRIO, LineFollow, NULL);
}

