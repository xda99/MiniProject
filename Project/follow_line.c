#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>

#define THRESHOLD_CURVE 				290 //number of pixels that define the beginning of a curve
#define CAMERA__DISTANCE_CORRECTION		450 //correction as the camera is not under the e-puck
#define ROTATION 						500 //for a 180 degrees rotation
#define TRESHOLD						10
#define WHEEL_DISTANCE					53 //mm
#define WHEEL_RAYON						40//mm

static THD_WORKING_AREA(waLineFollow, 256);
static THD_FUNCTION(LineFollow, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;
    //computes the speed to give to the motors
    int16_t speed = SPEED_EPUCK;
    int16_t speed_correction = 0;
    uint16_t position=0;

    while(1){
        time = chVTGetSystemTime();
        //computes a correction factor to let the robot rotate to be in front of the line
        speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

        //if the line is nearly in front of the camera, don't rotate
        if(abs(speed_correction) < ROTATION_THRESHOLD){
        	speed_correction = 0;
        	right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
        	left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
        }else if(get_line_width() > THRESHOLD_CURVE && (get_line_position() - (IMAGE_BUFFER_SIZE/2)) > 0){ //right curve
        	position=right_motor_get_pos();
			right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			do{
				right_motor_set_speed(speed);
				left_motor_set_speed(speed);
        	}while(abs(position-right_motor_get_pos())<CAMERA__DISTANCE_CORRECTION);


        	//e-puck turns until it detects the line
        	while(get_line_not_found() != LINE_FOUND){
        		virage();
        	}
        }else if(get_line_width() > THRESHOLD_CURVE && (get_line_position() - (IMAGE_BUFFER_SIZE/2)) < 0){ //left curve
        	position=right_motor_get_pos();
			right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			do{
				right_motor_set_speed(speed);
				left_motor_set_speed(speed);
        	}while(abs(position-right_motor_get_pos())<CAMERA__DISTANCE_CORRECTION);

			//e-puck turns until it detects the line
        	while(get_line_not_found() != LINE_FOUND){
        		virage();
        	}
        }else{
			right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
			left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
        }

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void line_follow_start(void){
	chThdCreateStatic(waLineFollow, sizeof(waLineFollow), NORMALPRIO, LineFollow, NULL);
}

void virage(void)
{
	int32_t position=0;
	int32_t x=0;
	int32_t y=0;
	int32_t angle=0;
	//int32_t hyp=0;
	bool right=false;
    int16_t speed = SPEED_EPUCK;

	//turn right
	if(get_line_width() > THRESHOLD_CURVE && (get_line_position() - (IMAGE_BUFFER_SIZE/2)) > 0)
	{
		position=left_motor_get_pos();
		right=true;
	}
	else if(get_line_width() > THRESHOLD_CURVE && (get_line_position() - (IMAGE_BUFFER_SIZE/2)) < 0)
	{
		position=left_motor_get_pos();
	}
	if((get_line_position()<(3.0/4.0)*IMAGE_BUFFER_SIZE+TRESHOLD) && (get_line_position()>(3.0/4.0)*IMAGE_BUFFER_SIZE-TRESHOLD))
	{
		x=left_motor_get_pos()-position;
		y=(3/4)*IMAGE_BUFFER_SIZE;
	}
	else if((get_line_position()<(1.0/4.0)*IMAGE_BUFFER_SIZE+TRESHOLD) && (get_line_position()>(1.0/4.0)*IMAGE_BUFFER_SIZE-TRESHOLD))
	{
		x=left_motor_get_pos()-position;
		y=(1/4)*IMAGE_BUFFER_SIZE;
	}

	angle=sin(y/x);
	//hyp=sqrt(x^2+y^2);

	//Speed correction to turn for "angle" on distance
	if(right)
	{
		 right_motor_set_speed(speed-(angle*WHEEL_DISTANCE)/WHEEL_RAYON);
	}
	else
	{
		left_motor_set_speed(speed-(angle*WHEEL_DISTANCE)/WHEEL_RAYON);
	}

}
