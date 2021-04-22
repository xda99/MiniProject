#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>

#define THRESHOLD_CURVE 	290 //number of pixels that define the beginning of a curve

static THD_WORKING_AREA(waLineFollow, 256);
static THD_FUNCTION(LineFollow, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;
    //computes the speed to give to the motors
    int16_t speed = SPEED_EPUCK;
    int16_t speed_correction = 0;
    uint16_t position_right=0;
    uint16_t position_left=0;

    while(1){
        time = chVTGetSystemTime();
        //computes a correction factor to let the robot rotate to be in front of the line
        speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

        //if the line is nearly in front of the camera, don't rotate
        if(abs(speed_correction) < ROTATION_THRESHOLD){
        	speed_correction = 0;
        	right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
        	left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
        }else if(get_line_width() > THRESHOLD_CURVE && (get_line_position() - (IMAGE_BUFFER_SIZE/2)) > 0){ //=> virage � droite
        	chprintf((BaseSequentialStream *)&SD3,"LineWidth = %d\n", get_line_width());
        	position_right=right_motor_get_pos();
        	position_left=left_motor_get_pos();
        	do{
				right_motor_set_pos(300);
				left_motor_set_pos(300);
				right_motor_set_speed(speed);
				left_motor_set_speed(speed);
        	}while(abs(position_left-left_motor_get_pos())<300);


        	//Faire tourner l'e-puck � droite jusqu'� ce qu'il d�tecte la ligne
        	while(get_line_not_found() != LINE_FOUND){
        	right_motor_set_pos(500);
        	left_motor_set_pos(500);
            right_motor_set_speed(-speed);
            left_motor_set_speed(speed);
            right_motor_get_pos();
            left_motor_get_pos();
        	}
        }else{
		//applies the speed from the PI regulator and the correction for the rotation
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
