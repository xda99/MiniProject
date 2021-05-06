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

/*
void go_along(void)
{
	uint16_t prox_left=0;
	uint16_t prox_right=0;
	float ds=0.0f;
	prox_left = (3*get_calibrated_prox(7) + 5*get_calibrated_prox(6) + get_calibrated_prox(5))/9;
	prox_right = (3*get_calibrated_prox(0) + 5*get_calibrated_prox(1) + get_calibrated_prox(2))/9;

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
		chprintf((BaseSequentialStream *)&SD3,"Obstacle\n");
	}
		//REGULATEUR
		//while(get_colors()!=BLACK)
		//{
			//right_motor_set_speed(pi_regulator());
			//left_motor_set_speed(pi_regulator());
		//}

	 obstacle=false;
}


int16_t pi_regulator(void)
{
	//	INT8?
	int16_t error = 0;
	int16_t speed = 0;
	static int16_t errord=0;
	static int16_t sum_error = 0;
	static systime_t time=0;

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

	speed = KP*error+KD*(error-errord)/time;//+  sum_error;//KP * error;/* + KI * sum_error*/

	chprintf((BaseSequentialStream *)&SD3,"Speed=%d\n", speed);
	errord=error;
	time = chVTGetSystemTime();

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
   // playSoundFileStart();				//Pour faire une mélodie depuis la carte SD?!
   // setSoundFileVolume(VOLUME_MAX);
    playMelodyStart();					//Pour faire une mélodie depuis le code
   // pi_regulator_start();


    // - IR0 (front-right) + IR4 (back-left)
    // - IR1 (front-right-45deg) + IR5 (left)
    // - IR2 (right) + IR6 (front-left-45deg)
    // - IR3 (back-right) + IR7 (front-left)
    int16_t speed_correction=0;
    int16_t speed=0;

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
			chprintf((BaseSequentialStream *)&SD3,"Obstacle\n");
		}

    	 if(obstacle_on_side)
    	 {
    		 speed_correction=pi_regulator();

			 //computes a correction factor to let the robot rotate to be in front of the line
			 if(left)
			 {
//				 speed_correction = abs(IR_VALUE-get_calibrated_prox(2));
//				 if(get_calibrated_prox(0)>get_calibrated_prox(3))
//				 {
//					 	chprintf((BaseSequentialStream *)&SD3,"1\n");
						right_motor_set_speed(SPEED_EPUCK+speed_correction);
						left_motor_set_speed(SPEED_EPUCK-speed_correction);
/*				 }
				 else
				 {
					 	chprintf((BaseSequentialStream *)&SD3,"2\n");
						right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
						left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
				 }*/
			 }
			 else
			 {
				// speed_correction = abs(IR_VALUE-get_calibrated_prox(5));
				// if(get_calibrated_prox(7)>get_calibrated_prox(4))
				// {
					 //	chprintf((BaseSequentialStream *)&SD3,"speed=%d\n",speed_correction);
						right_motor_set_speed(SPEED_EPUCK-speed_correction);
						left_motor_set_speed(SPEED_EPUCK+speed_correction);
				// }
				 /*else
				 {
					 	chprintf((BaseSequentialStream *)&SD3,"4\n");
						right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
						left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
				 }*/
			 }
    	 }
    }
}

void skirt_start(void)
{
	chThdCreateStatic(waSkirt, sizeof(waSkirt), NORMALPRIO, Skirt, NULL);
}
