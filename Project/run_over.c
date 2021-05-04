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

/*
void go_along(void)
{
	uint16_t prox_left=0;
	uint16_t prox_right=0;
	float ds=0.0f;
	prox_left = (3*get_prox(7) + 5*get_prox(6) + get_prox(5))/9;
	prox_right = (3*get_prox(0) + 5*get_prox(1) + get_prox(2))/9;

	//obstacle on the right side
	if(prox_right>prox_left)
	{
		ds = EPUCK_SPEED * (prox_right / IR_VALUE);
		left_motor_set_speed(2*EPUCK_SPEED-ds);
		right_motor_set_speed(ds);
	}
	else
	{
		ds = EPUCK_SPEED * (prox_left / IR_VALUE);
		right_motor_set_speed(2*EPUCK_SPEED-ds);
		left_motor_set_speed(ds);
	}
}
*/




void go_along(void)
{
	bool left=false;

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

	if((get_prox(2)>60 || get_prox(5)>60) && !obstacle_on_side)
	{
		obstacle_on_side=true;
	}
		//REGULATEUR
		//while(get_colors()!=BLACK)
		//{
			//right_motor_set_speed(pi_regulator());
			//left_motor_set_speed(pi_regulator());
		//}

	 obstacle=false;
}


int16_t pi_regulator(void){

	float error = 0;
	float speed = 0;

	static float sum_error = 0;

	if(turn_left())
	{
		error = get_prox(2)-IR_VALUE;
	}
	else
	{
		error = get_prox(5)-IR_VALUE;
	}

	//disables the PI regulator if the error is to small
	//this avoids to always move as we cannot exactly be where we want and
	//the camera is a bit noisy
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

	speed = KP * error + KI * sum_error;

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
		if(get_prox(0)>get_prox(7))
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
    pi_regulator_start();


    // - IR0 (front-right) + IR4 (back-left)
    // - IR1 (front-right-45deg) + IR5 (left)
    // - IR2 (right) + IR6 (front-left-45deg)
    // - IR3 (back-right) + IR7 (front-left)
    int16_t speed_correction=0;
    int16_t speed=0;

    while(1)
    {
    	// chprintf((BaseSequentialStream *)&SD3,"%d\n", get_prox(7));
    	 if(get_prox(0)>60 || get_prox(7)>60)
    	 {
    		 obstacle=true;
    	 }
    	 if(obstacle)
    	 {
    		 go_along();
    		 speed=pi_regulator();


			 //computes a correction factor to let the robot rotate to be in front of the line
			 if(turn_left())
			 {
				 speed_correction = IR_VALUE-get_prox(2);
				 if(get_prox(0)>get_prox(3))
				 {
						right_motor_set_speed(speed - 500 + ROTATION_COEFF * speed_correction);
						left_motor_set_speed(speed - 500 - ROTATION_COEFF * speed_correction);
				 }
				 else
				 {
						right_motor_set_speed(speed - 500 - ROTATION_COEFF * speed_correction);
						left_motor_set_speed(speed - 500 + ROTATION_COEFF * speed_correction);
				 }
			 }
			 else
			 {
				 speed_correction = IR_VALUE-get_prox(5);
				 if(get_prox(7)>get_prox(4))
				 {
						right_motor_set_speed(speed - 500 + ROTATION_COEFF * speed_correction);
						left_motor_set_speed(speed - 500 - ROTATION_COEFF * speed_correction);
				 }
				 else
				 {
						right_motor_set_speed(speed - 500 - ROTATION_COEFF * speed_correction);
						left_motor_set_speed(speed - 500 + ROTATION_COEFF * speed_correction);
				 }

			 }

			 //if the line is nearly in front of the camera, don't rotate
			 if(abs(speed_correction) < CORRECTION_THRESHOLD)
			 {
				speed_correction = 0;
			 }

			 //applies the speed from the PI regulator and the correction for the rotation
			right_motor_set_speed(speed - 500 - ROTATION_COEFF * speed_correction);
			left_motor_set_speed(speed - 500 + ROTATION_COEFF * speed_correction);
    	 }
    }
}

void skirt_start(void)
{
	chThdCreateStatic(waSkirt, sizeof(waSkirt), NORMALPRIO, Skirt, NULL);
}

