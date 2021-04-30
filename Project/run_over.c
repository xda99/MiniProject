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

#define RIGHT	SPEED_EPUCK
#define	LEFT	- SPEED_EPUCK

static bool obstacle=false;

void go_along(void)
{
	bool left=false;

	playNote(440, 1000);

//       do
//       {

	left=turn_left();

	if(left)
	{
		rotation(LEFT);
	}
	else
	{
		rotation(RIGHT);
	}

	if(get_prox(2)<15 || get_prox(5)<15)
	{
		while(get_colors()!=BLACK)
		{
			if(left)
			{
			//	pi_regulator(15.0f, get_prox(2));
			}
			else
			{
			//	pi_regulator(15.0f, get_prox(5));
			}
		}

	}
	 obstacle=false;
//        }
//        while((get_prox(2) || get_prox(5))< 15);

		//Régulateur pour qu'il reste à distance en contournant
}
     //   while(get_color()!=BLACK);

void rotation(int16_t direction)
{
    right_motor_set_speed(-direction);
    left_motor_set_speed(direction);
}

bool turn_left(void)
{
	if(get_prox(0)<get_prox(7))
	{
		return true;
	}
	else
	{
		return false;
	}
}
/*
void go_along(void)
{
	int16_t err=0;
	if(turn_left())
	{

	}
	else
	{

	}
}
*/

static THD_WORKING_AREA(waSkirt, 256);
static THD_FUNCTION(Skirt, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

   //Thread for the claxon!
   // playSoundFileStart();				//Pour faire une mélodie depuis la carte SD?!
   // setSoundFileVolume(VOLUME_MAX);
    playMelodyStart();					//Pour faire une mélodie depuis le code
    pi_regulator_start();


    // - IR0 (front-right) + IR4 (back-left)
    // - IR1 (front-right-45deg) + IR5 (left)
    // - IR2 (right) + IR6 (front-left-45deg)
    // - IR3 (back-right) + IR7 (front-left)

    while(1)
    {
    	// chprintf((BaseSequentialStream *)&SD3,"%d\n", get_prox(7));
    	 if(get_prox(0)<10 && get_prox(7)<10)
    	 {
    		 obstacle=true;
    	 }
    	 if(obstacle)
    	 {
    		 go_along();
    	 }
    }
}

void skirt_start(void)
{
	chThdCreateStatic(waSkirt, sizeof(waSkirt), NORMALPRIO, Skirt, NULL);
}

