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

//number of pixels that define the beginning of a curve
#define THRESHOLD_CURVE 				290
#define	DISTANCE_CURVATURE				40
#define	CURVE_CORRECTION				80

//To make the epuck move forward a bit when it loses the line [mm]
#define	DISTANCE						30

static bool turn=false;
static bool right=false;
static bool begin_turn=false;
static bool position_done=false;
static uint16_t speed_virage_corr = 0;
static int16_t speed_r=0;
static int16_t speed_l=0;


//distance in mm, speed in step/s
void position(float distance, int16_t speed)
{
	left_motor_set_pos(0);
	speed_l=speed;
	speed_r=speed;

	//If the distance in straight line is reached, set a bool to true
	if(left_motor_get_pos()*0.13f>distance)
	{
		position_done=true;
	}
}

void curve(void)
{
	//Curve is on the right
	if(((get_line_position()-(IMAGE_BUFFER_SIZE/2))>0) && !turn)
	{
		right=true;
	}
	//Curve is on the left
	else if(((get_line_position()-(IMAGE_BUFFER_SIZE/2))<0) && !turn)
	{
		right=false;
	}

	//Set the postion of the motors if it is the first time the function is called
	if(!turn)
	{
		turn=true;
		right_motor_set_pos(0);
	}

	//Set a bool to true to let the robot start to turn
	if((right_motor_get_pos()>CAMERA__DISTANCE_CORRECTION) && !begin_turn)
	{
		begin_turn=true;
	}

	//The epuck continues to go in a straight line to avoid it to turn too early
	if(!begin_turn)
	{
		speed_r=SPEED_EPUCK;
		speed_l=SPEED_EPUCK;
	}

	//To know an approximation of the curvature of the turn
	if(right_motor_get_pos()==DISTANCE_CURVATURE)
	{
		speed_virage_corr=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2));
	}

	//Correct the speed in function of the sense of the curvature
	if(right && turn && begin_turn)
	{
		speed_r=SPEED_EPUCK-2*speed_virage_corr-CURVE_CORRECTION;
		speed_l=SPEED_EPUCK+2*speed_virage_corr+CURVE_CORRECTION;
	}
	else if(!right && turn && begin_turn)
	{
		speed_r=SPEED_EPUCK+2*speed_virage_corr+CURVE_CORRECTION;
		speed_l=SPEED_EPUCK-2*speed_virage_corr-CURVE_CORRECTION;
	}

	if(get_line_not_found() == LINE_FOUND)
	{
		turn=false;
		begin_turn=false;
		position_done=false;
	}
}

void straight_line(int16_t speed_correction)
{
	//Continue to move forward a little bit when the epuck lose the line
	if((get_line_not_found() != LINE_FOUND) && !position_done)
	{
		position(DISTANCE, SPEED_EPUCK);
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

    int16_t speed_correction = 0;

    while(1)
    {
    	if(!return_obstacle())
    	{
			speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

			//If the line is nearly in front of the camera, don't rotate
			if(((abs(speed_correction) < ROTATION_THRESHOLD) && !turn))
			{
				speed_r=SPEED_EPUCK;
				speed_l=SPEED_EPUCK;
			}
			else if((get_line_width() > THRESHOLD_CURVE || turn))
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

