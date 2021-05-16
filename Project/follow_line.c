/**
 * @file    follow_line.c
 * @brief   Everything that keep the robot in the center of the line
 *
 * @author  David Niederhauser and Valentin Roch
 */
#include <main.h>
#include <follow_line.h>
#include <motors.h>
#include <run_over.h>
#include <process_image.h>

//Number of pixels that define the beginning of a curve
#define THRESHOLD_CURVE 				290

//Distance used to get a second value of the line position for the speed correction
#define	DISTANCE_CURVATURE				40
#define	CURVE_SPEED_CORR				80

//To make the epuck move forward a bit when it loses the line [mm]
#define	DISTANCE						30

static bool turn=false;
static bool right=false;
static bool begin_turn=false;
static bool position_done=false;
static uint16_t speed_virage_corr = 0;
static int16_t speed_r=0;
static int16_t speed_l=0;

void position(float distance, int16_t speed)
{
	left_motor_set_pos(0);
	speed_l=speed;
	speed_r=speed;

	//If the distance in straight line is reached, set a bool to true
	if(left_motor_get_pos()*STEP_TO_MM>distance)
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

	//Sets the position of the motors if it is the first time the function is called
	if(!turn)
	{
		turn=true;
		right_motor_set_pos(0);
	}

	//Sets a bool to true to let the robot start to turn
	if((right_motor_get_pos()>CAMERA__DISTANCE_CORRECTION) && !begin_turn)
	{
		begin_turn=true;
	}

	//The epuck continues to go in a straight line to avoid to turn too early
	if(!begin_turn)
	{
		speed_r=SPEED_EPUCK;
		speed_l=SPEED_EPUCK;
	}

	//To know an approximation of the curvature of the curve
	if(right_motor_get_pos()==DISTANCE_CURVATURE)
	{
		speed_virage_corr=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2));
	}

	//Corrects the speed in function of the sense of the curvature
	if(right && turn && begin_turn)
	{
		speed_r=SPEED_EPUCK-2*speed_virage_corr-CURVE_SPEED_CORR;
		speed_l=SPEED_EPUCK+2*speed_virage_corr+CURVE_SPEED_CORR;
	}
	else if(!right && turn && begin_turn)
	{
		speed_r=SPEED_EPUCK+2*speed_virage_corr+CURVE_SPEED_CORR;
		speed_l=SPEED_EPUCK-2*speed_virage_corr-CURVE_SPEED_CORR;
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
	//Continues to move forward a little bit when the epuck loses the line
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

static THD_WORKING_AREA(waLineFollow, 256);
static THD_FUNCTION(LineFollow, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    int16_t speed_correction = 0;

    while(1)
    {
    	if(!return_obstacle())
    	{
			speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

			//Don't rotate if the epuck is almost aligned
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

