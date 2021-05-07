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
#include <pi_regulator.h>
#include <main.h>
#include <audio/play_melody.h>
#include <run_over.h>
#include <follow_line.h>

#define 	RIGHT					SPEED_EPUCK
#define		LEFT					-SPEED_EPUCK
#define 	CORRECTION_THRESHOLD	10

static bool obstacle=false;
static bool obstacle_on_side=false;
static bool left=false;

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

	if((get_calibrated_prox(2)>IR_VALUE || get_calibrated_prox(5)>IR_VALUE) && !obstacle_on_side)
	{
		obstacle_on_side=true;
		//chprintf((BaseSequentialStream *)&SD3,"Obstacle\n");
	}
	 obstacle=false;
}

//changer le nom
int16_t pi_regulator(void)
{
	//	INT8?
	int16_t error = 0;
	int16_t speed = 0;
	static int16_t errord=0;
	static int16_t sum_error = 0;
//	static systime_t time=0;


	if(left)
	{
		error = get_calibrated_prox(2)-IR_VALUE;
	}
	else
	{
		error = get_calibrated_prox(5)-IR_VALUE;
	}

	//disables the PI regulator if the error is to small
	if(abs(error) < ERROR_THRESHOLD)
	{
		return 0;
	}

	sum_error += error;
	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR)
	{
		sum_error = MAX_SUM_ERROR;
	}
	else if(sum_error < -MAX_SUM_ERROR)
	{
		sum_error = -MAX_SUM_ERROR;
	}

	speed = KP*error;	//+KD*(error-errord);	//+  sum_error;//KP * error;/* + KI * sum_error*/

	errord=error;
//	time = chVTGetSystemTime();
/*
	if(left && (get_calibrated_prox(1)>IR_VALUE && get_calibrated_prox(2)<10 && obstacle_on_side))
	{
		chprintf((BaseSequentialStream *)&SD3,"1\n");
		speed=0;
	}
	else if(!left && (get_calibrated_prox(6)>IR_VALUE && get_calibrated_prox(5)<10 && obstacle_on_side))
	{
		chprintf((BaseSequentialStream *)&SD3,"2\n");
		speed=0;
	}
*/
    return (int16_t)speed;
}

void rotation(int16_t direction)
{
    right_motor_set_speed(-direction);
    left_motor_set_speed(direction);
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

   //Thread for the claxon!
   // playSoundFileStart();				//Pour faire une m�lodie depuis la carte SD?!
   // setSoundFileVolume(VOLUME_MAX);
    playMelodyStart();					//Pour faire une m�lodie depuis le code
   // pi_regulator_start();


    // - IR0 (front-right) + IR4 (back-left)
    // - IR1 (front-right-45deg) + IR5 (left)
    // - IR2 (right) + IR6 (front-left-45deg)
    // - IR3 (back-right) + IR7 (front-left)
    int16_t speed_correction=0;

    while(1)
    {
    	// chprintf((BaseSequentialStream *)&SD3,"%d\n", get_calibrated_prox(7));
    	 if(get_calibrated_prox(0)>IR_VALUE || get_calibrated_prox(7)>IR_VALUE)
    	 {
    		 obstacle=true;
    	 }
    	 if(obstacle)
    	 {
    		 go_along();
    	 }

		if((get_calibrated_prox(2)>IR_VALUE|| get_calibrated_prox(5)>IR_VALUE) && !obstacle_on_side)
		{
			obstacle_on_side=true;
		}

	    if(obstacle_on_side)
	    {
			 speed_correction=pi_regulator();
			 //computes a correction factor to let the robot rotate to be in front of the line
			 if(left)
			 {
			  //chprintf((BaseSequentialStream *)&SD3,"1\n");
				right_motor_set_speed(SPEED_EPUCK+speed_correction);
				left_motor_set_speed(SPEED_EPUCK-speed_correction);
			 }
			 else
			 {
				right_motor_set_speed(SPEED_EPUCK-speed_correction);
				left_motor_set_speed(SPEED_EPUCK+speed_correction);
			 }
	    }
    }
}

void skirt_start(void)
{
	chThdCreateStatic(waSkirt, sizeof(waSkirt), NORMALPRIO, Skirt, NULL);
}
