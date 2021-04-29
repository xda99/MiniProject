#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>
#include <follow_line.h>

#define THRESHOLD_CURVE 				290		//number of pixels that define the beginning of a curve
#define ROTATION 						500		//for a 180 degrees rotation
#define TRESHOLD						10
#define WHEEL_RAYON						40 		//mm
#define PIXEL_SIZE_MM					0.0028f  //mm
#define WHEEL_DISTANCE     				53.5f    //mm



#define POSITION_NOT_REACHED	0
#define POSITION_REACHED		1


//position_r and position_l in mm
void position(float distance, int16_t speed)
{
	int16_t right_init=right_motor_get_pos();

	do
	{
		left_motor_set_speed(speed);
		right_motor_set_speed(speed);
	}while(abs(right_init-right_motor_get_pos())*0.13f<distance);
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void angle_rotation(float angle, int16_t speed_r)
{
	int16_t right_init=right_motor_get_pos();
	int16_t left_init=left_motor_get_pos();
	do
	{
		left_motor_set_speed(speed_r);
		right_motor_set_speed(-speed_r);
	}while(((float)abs(right_init-right_motor_get_pos())*0.13f <(WHEEL_DISTANCE/2)*angle) && ((float)abs(left_init-left_motor_get_pos())*0.13f <WHEEL_DISTANCE*angle/2));

	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

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
    uint16_t pos=0;

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
    	position((float)THRESHOLD_CURVE, SPEED_EPUCK);

    	//chprintf((BaseSequentialStream *)&SD3,"Right");
    	pos=right_motor_get_pos();
		//	right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
		//	left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);

		right_motor_set_speed(speed);
		left_motor_set_speed(speed);
			if(abs(pos-right_motor_get_pos())==30)
			{
				x=abs(left_motor_get_pos()-pos)*0.13f; //mm
				y=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2))*PIXEL_SIZE_MM;	//mm
			}

		angle=atan(y/x);
		hyp=sqrt(x*x+y*y);

		do
		{
			angle_rotation(angle, speed);
			position(hyp, speed);

		}while(get_line_not_found() != LINE_FOUND);
    }
    else if(get_line_width() > THRESHOLD_CURVE && speed_correction < 0) //left curve
    { 
    	position((float)THRESHOLD_CURVE, SPEED_EPUCK);
    	pos=right_motor_get_pos();
		//right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
		//left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
		right_motor_set_speed(speed);
		left_motor_set_speed(speed);

		if(abs(pos-right_motor_get_pos())==30)
		{
			x=abs(left_motor_get_pos()- pos)*0.13f; //mm
			//RAJOUTER LA CONDITION SUR L'AVANCEE DE X ET PRENDRE Y DU CODE
			y=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2))*PIXEL_SIZE_MM;	//mm
	     }

		angle=atan(y/x);
		hyp=sqrt(x*x+y*y);

		//Speed correction to turn for "angle" on distance
		do
		{
			angle_rotation(angle, -speed);
			position(hyp, speed);
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
      
        //virage();
        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void line_follow_start(void){
	chThdCreateStatic(waLineFollow, sizeof(waLineFollow), NORMALPRIO, LineFollow, NULL);
}


