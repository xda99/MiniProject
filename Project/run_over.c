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

#define RIGHT	SPEED_EPUCK
#define	LEFT	- SPEED_EPUCK

void run_over(void)
{

    //Thread for the IR distance sensor
    proximity_start();

   //Thread for the claxon!
   // playSoundFileStart();				//Pour faire une mélodie depuis la carte SD?!
   // setSoundFileVolume(VOLUME_MAX);
    playMelodyStart();					//Pour faire une mélodie depuis le code



    // - IR0 (front-right) + IR4 (back-left)
    // - IR1 (front-right-45deg) + IR5 (left)
    // - IR2 (right) + IR6 (front-left-45deg)
    // - IR3 (back-right) + IR7 (front-left)
    if((get_prox(0) && get_prox(7))<10)
    {
        playNote(440, 1000);

//       do
//       {


		if(turn_left())
		{
			rotation(LEFT);
		}
		else
		{
			rotation(RIGHT);
		}
		if((get_prox(2) || get_prox(5))< 15)
		{
			//Stop de tourner
		}
//        }
//        while((get_prox(2) || get_prox(5))< 15);

        	//Régulateur pour qu'il reste à distance en contournant
        }
     //   while(get_color()!=BLACK);
    if(get_colors()==BLACK)
    {
    	//Stopper le thread de regulateur
	}
}

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
