#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>

#define THRESHOLD_CURVE 				290		//number of pixels that define the beginning of a curve
#define ROTATION 						500		//for a 180 degrees rotation
#define TRESHOLD						10
#define WHEEL_DISTANCE					53 		//mm
#define WHEEL_RAYON						40 		//mm
#define PIXEL_SIZE_MM					2.8e-3  //mm

void virage(void)
{
	float x=0;
	float y=0;
	float angle=0;
	float hyp=0;
	//int32_t t=0;
    //computes the speed to give to the motors
    int16_t speed = SPEED_EPUCK;
    int16_t speed_correction = 0;
    uint16_t position=0;

  	//computes a correction factor to let the robot rotate to be in front of the line
    speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

    //if the line is nearly in front of the camera, don't rotate
    if(abs(speed_correction) < ROTATION_THRESHOLD){
    	speed_correction = 0;
    	right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
    	left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
    }
    else if(get_line_width() > THRESHOLD_CURVE && speed_correction > 0) //right curve
    { 
    	chprintf((BaseSequentialStream *)&SD3,"Right");
    	position=right_motor_get_pos();
		//	right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
		//	left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);

		right_motor_set_speed(speed);
		left_motor_set_speed(speed);
		do{
			if(abs(position-right_motor_get_pos())==30)
			{
				x=abs(left_motor_get_pos()-position)*(130.0f/1000.0f); //mm
				y=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2))*PIXEL_SIZE_MM;	//mm
			}
    	}while(abs(position-right_motor_get_pos())<CAMERA__DISTANCE_CORRECTION);

		angle=atan(y/x);
		hyp=sqrt(x*x+y*y);

		do
		{
			 right_motor_set_speed(speed-2*speed_correction);
			 left_motor_set_speed(speed+2*speed_correction);

		}while(get_line_not_found() != LINE_FOUND);
    }
    else if(get_line_width() > THRESHOLD_CURVE && speed_correction < 0) //left curve
    { 
    	position=right_motor_get_pos();
		//right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
		//left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
		right_motor_set_speed(speed);
		left_motor_set_speed(speed);

		do{
			if(abs(position-right_motor_get_pos())==30)
			{
				x=abs(left_motor_get_pos()-position)*(130.0f/1000.0f); //mm
				//RAJOUTER LA CONDITION SUR L'AVANCEE DE X ET PRENDRE Y DU CODE
				y=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2))*PIXEL_SIZE_MM;	//mm
			}
	     }while(abs(position-right_motor_get_pos())<CAMERA__DISTANCE_CORRECTION);

		angle=atan(y/x);
		hyp=sqrt(x*x+y*y);

		//Speed correction to turn for "angle" on distance
		do
		{
			 left_motor_set_speed(speed-2*speed_correction);
			 right_motor_set_speed(speed+2*speed_correction);
		}while(get_line_not_found() != LINE_FOUND);
    }
    else
    {
		right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
		left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
    }
}


static THD_WORKING_AREA(waLineFollow, 2048);
static THD_FUNCTION(LineFollow, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;
    while(1){
        time = chVTGetSystemTime();
      
        virage();
        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void line_follow_start(void){
	chThdCreateStatic(waLineFollow, sizeof(waLineFollow), NORMALPRIO, LineFollow, NULL);
}


