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
#define ROTATION 						500 //for a 180 degrees rotation
#define TRESHOLD						10
#define WHEEL_DISTANCE					53 //mm
#define WHEEL_RAYON						40//mm


static THD_WORKING_AREA(waLineFollow, 2048);
static THD_FUNCTION(LineFollow, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;
    //computes the speed to give to the motors
    int16_t speed = SPEED_EPUCK;
    int16_t speed_correction = 0;
    uint16_t speed_virage_cor = 0;

    while(1){
        time = chVTGetSystemTime();
        //computes a correction factor to let the robot rotate to be in front of the line
        speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

        //if the line is nearly in front of the camera, don't rotate
        if(abs(speed_correction) < ROTATION_THRESHOLD){
        	speed_correction = 0;
        	right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
        	left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
        }else if(get_line_width() > THRESHOLD_CURVE) //=>virage
        {
        	right_motor_set_pos(0);
        	left_motor_set_pos(0);
			do{
				right_motor_set_speed(speed);
				left_motor_set_speed(speed);

				if(right_motor_get_pos()==40)
				{
					speed_virage_cor=abs(get_line_position()-(IMAGE_BUFFER_SIZE/2));
				}
        	}while(right_motor_get_pos()<CAMERA__DISTANCE_CORRECTION);
//			chprintf((BaseSequentialStream *)&SD3,"right= %d \n",speed_virage_cor);

			if((get_line_position()-(IMAGE_BUFFER_SIZE/2))>0)//right curve
			{
				right_motor_set_speed(speed-2*speed_virage_cor-80);
				left_motor_set_speed(speed+2*speed_virage_cor+80);
				while(get_line_not_found() != LINE_FOUND);
			}
			else if((get_line_position()-(IMAGE_BUFFER_SIZE/2))<0) //left curve
			{
				right_motor_set_speed(speed+2*speed_virage_cor+80);
				left_motor_set_speed(speed-2*speed_virage_cor-80);
				while(get_line_not_found() != LINE_FOUND);
			}

        }
        else
        {
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


