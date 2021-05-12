#include <sensors/proximity.h>
#include <ch.h>
#include <hal.h>
#include "audio/microphone.h"
#include "audio/mp45dt02_processing.h"
#include <fat.h>
#include <audio/audio_thread.h>

#include <chprintf.h>
#include <motors.h>
#include <colors.h>
#include <main.h>
#include <audio/play_melody.h>
#include <run_over.h>
#include <follow_line.h>
#include <process_image.h>

#define 	RIGHT					SPEED_EPUCK
#define		LEFT					-SPEED_EPUCK
#define 	CORNER					10
#define		ANGLE					1.57
#define		STEP_TO_MM				0.13f
#define		DISTANCE				40

static bool obstacle=false;
static bool obstacle_on_side=false;
static bool left=false;
static bool corner=false;
static int16_t speed_r=0;
static int16_t speed_l=0;


void go_along(void)
{
	//playNote(440, 1000);

	left=turn_left();

	if(left && !obstacle_on_side)
	{
		rotation(LEFT);
	}
	else if(!obstacle_on_side)
	{
		rotation(RIGHT);
	}
}

int16_t regulator(void)
{
	//	INT8?
	int16_t error = 0;
	int16_t speed = 0;

	if(left)
	{
		error = (get_calibrated_prox(2)-IR_VALUE)+(get_calibrated_prox(1)-(IR_VALUE+10)/2);
	}
	else
	{
		error = get_calibrated_prox(5)-IR_VALUE+(get_calibrated_prox(6)-(IR_VALUE+10)/2);
	}

	//disables the PI regulator if the error is to small
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

static THD_WORKING_AREA(waSkirt, 2048);
static THD_FUNCTION(Skirt, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

   //Thread for the claxon!
   // playSoundFileStart();				//Pour faire une mélodie depuis la carte SD?!
   // setSoundFileVolume(VOLUME_MAX);
    playMelodyStart();					//Pour faire une mélodie depuis le code

    // - IR0 (front-right) + IR4 (back-left)
    // - IR1 (front-right-45deg) + IR5 (left)
    // - IR2 (right) + IR6 (front-left-45deg)
    // - IR3 (back-right) + IR7 (front-left)
    int16_t speed_correction=0;

    while(1)
    {
    	 if((get_calibrated_prox(0)>IR_VALUE || get_calibrated_prox(7)>IR_VALUE) && !obstacle_on_side) //=> obstacle
    	 {
    		 obstacle=true;
    		 go_along();
   	 	 }

    	if((get_calibrated_prox(2)>(IR_VALUE-10) || get_calibrated_prox(5)>(IR_VALUE-10)) && !obstacle_on_side)
    	{
    		obstacle_on_side=true;
    	}

	    if(obstacle_on_side)
	    {
			 speed_correction=regulator();
			 //computes a correction factor to let the robot rotate to be in front of the line
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
	    	corner=false;
	    }
//Commenter
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

