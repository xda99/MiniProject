#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>
#include <motors.h>

#include <process_image.h>

#include <pal.h>

#include <colors.h>

#define 	LINE_SIZE				175//40
#define		THRESHOLD_RED			130//140
#define		THRESHOLD_GREEN			24//22//27
#define		THRESHOLD_BLUE			16
#define 	THRESHOLD_BLACK			10

static int16_t	speed_r=0;
static int16_t	speed_l=0;
static bool color_detected=false;
static int16_t speed_reduction=SPEED_EPUCK;

uint8_t get_colors(void)
{
	uint8_t red[IMAGE_BUFFER_SIZE] = {0};
	uint8_t green[IMAGE_BUFFER_SIZE] = {0};
	uint8_t blue[IMAGE_BUFFER_SIZE] = {0};
	uint8_t *img_buff_ptr;
	uint16_t compt_red=0;
	uint16_t compt_green=0;
	uint16_t compt_blue=0;
	uint16_t compt_black=0;

	img_buff_ptr = dcmi_get_last_image_ptr();

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
		{
			red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;

			if(red[i/2]>THRESHOLD_RED)
			{
				compt_red+=1;
			}
			if(red[i/2]<THRESHOLD_BLACK)
			{
				compt_black+=1;
			}

			green[i/2] = (((uint8_t)img_buff_ptr[i+1]&0xE0)>>5) + (((uint8_t)img_buff_ptr[i]&0x07)<<3);

			if(green[i/2]>THRESHOLD_GREEN && red[i/2]<65)
			{
				compt_green+=1;
			}

			blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;

			if(blue[i/2]>THRESHOLD_BLUE)
			{
				compt_blue+=1;
			}
		}

	if(compt_red>LINE_SIZE)
	{
		color_detected=true;
		return RED;
	}
	else if(compt_green>LINE_SIZE)
	{
		color_detected=false;
		return GREEN;
	}
/*	else if(compt_blue>LINE_SIZE)
	{
		return BLUE;
	}
	else if(compt_black>LINE_SIZE)
	{
		return BLACK;
	}
	else if(compt_yellow>LINE_SIZE)
	{
		return YELLOW;
	}*/
	else
	{
		return NO_COLOR;
	}
}

static THD_WORKING_AREA(waColorDetection, 2048);
static THD_FUNCTION(ColorDetection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;


    while(1)
    {
    	uint8_t color = get_colors();

    	if(color==RED)
    	{
			speed_r=0;
			speed_l=0;
    	}
    	if(color==GREEN)
		{
			speed_r=SPEED_EPUCK;
			speed_l=SPEED_EPUCK;
		}
    	if(color==BLUE)
    	{
    		speed_reduction=SPEED_EPUCK/2;
    	}
    	else
    	{
    		speed_reduction=SPEED_EPUCK;
    	}/*
      	if(color==BLACK)
    	{
    		chprintf((BaseSequentialStream *)&SD3,"black\n");
    	}
     	if(color==YELLOW) // Passage pieton => Ralenti
		{
    		position=right_motor_get_pos();
			right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			do{
				right_motor_set_speed(SPEED_EPUCK-100);
				left_motor_set_speed(SPEED_EPUCK-100);
			}while(abs(position-right_motor_get_pos())<CAMERA__DISTANCE_CORRECTION);
		}*/
    	chThdSleepMilliseconds(10);
 //   	 chThdYield();
    }
}

int16_t return_speed_r_c(void)
{
	return speed_r;
}
int16_t return_speed_l_c(void)
{
	return speed_l;
}

void color_detection_start(void)
{
	chThdCreateStatic(waColorDetection, sizeof(waColorDetection), NORMALPRIO+1, ColorDetection, NULL);
}

bool return_color_detected(void)
{
	return color_detected;
}

int16_t return_speed_reduction(void)
{
	return speed_reduction;
}
