/**
 * @file    run_over.c
 * @brief	Detects if there is an obstacle in front of the epuck and go along it if so
 *
 *	 As a reminder for the numbers of the IR sensors from the library e-puck2_main-processor/src/sensors/proximity.c
 *   - IR0 (front-right) + IR4 (back-left)
 *   - IR1 (front-right-45deg) + IR5 (left)
 *   - IR2 (right) + IR6 (front-left-45deg)
 *   - IR3 (back-right) + IR7 (front-left)
 *
 * @author  David Niederhauser and Valentin Roch
 */

#include <audio/audio_thread.h>
#include <main.h>
#include <run_over.h>
#include <sensors/proximity.h>
#include <process_image.h>

#define 	RIGHT					SPEED_EPUCK
#define		LEFT					-SPEED_EPUCK
#define 	IR_THRESHOLD			120
#define     FRONT_IR_THRESHOLD		(IR_THRESHOLD+10)/2
#define		KP						4
#define		ERROR_THRESHOLD			10

static bool obstacle=false;
static bool obstacle_on_side=false;
static bool left=false;
static int16_t speed_r=0;
static int16_t speed_l=0;

void go_along(void)
{
	dac_play(800);

	left=turn_left();

	if(left)
	{
		rotation(LEFT);
	}
	else
	{
		rotation(RIGHT);
	}
}

int16_t regulator(void)
{
	int16_t error = 0;
	int16_t speed = 0;

	if(left)
	{
		error = (get_calibrated_prox(2)-IR_THRESHOLD)+(get_calibrated_prox(1)-FRONT_IR_THRESHOLD);
	}
	else
	{
		error = get_calibrated_prox(5)-IR_THRESHOLD+(get_calibrated_prox(6)-FRONT_IR_THRESHOLD);
	}

	if(abs(error) < ERROR_THRESHOLD)
	{
		return 0;
	}

	speed = KP*error;
    return (int16_t)speed;
}

void rotation(int16_t direction)
{
    speed_r=-direction;
    speed_l=direction;
}

bool turn_left(void)
{
	if(obstacle)
	{
		if(get_calibrated_prox(0)>get_calibrated_prox(7))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

static THD_WORKING_AREA(waSkirt, 256);
static THD_FUNCTION(Skirt, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    int16_t speed_correction=0;

    while(1)
    {
    	if((get_calibrated_prox(0)>IR_THRESHOLD || get_calibrated_prox(7)>IR_THRESHOLD) && !obstacle_on_side)
    	{
    		obstacle=true;
    		go_along();
   	 	}

    	if((get_calibrated_prox(2)>(IR_THRESHOLD-ERROR_THRESHOLD) ||
    		get_calibrated_prox(5)>(IR_THRESHOLD-ERROR_THRESHOLD)) && !obstacle_on_side)
    	{
    		dac_stop();
    		obstacle_on_side=true;
    	}

	    if(obstacle_on_side)
	    {
			 speed_correction=regulator();
			 if(left)
			 {
					 speed_r=SPEED_EPUCK+speed_correction;
					 speed_l=SPEED_EPUCK-speed_correction;
			 }
			 else
			 {
					 speed_r=SPEED_EPUCK-speed_correction;
					 speed_l=SPEED_EPUCK+speed_correction;
			 }
	    }

	    if(get_line_not_found()==LINE_FOUND)
	    {
	    	obstacle=false;
	    	obstacle_on_side=false;
	    }
	    chThdYield();
    }
}
int16_t return_speed_r_ro(void)
{
	return speed_r;
}

int16_t return_speed_l_ro(void)
{
	return speed_l;
}

bool return_obstacle(void)
{
	return obstacle;
}

void skirt_start(void)
{
	chThdCreateStatic(waSkirt, sizeof(waSkirt), NORMALPRIO, Skirt, NULL);
}

