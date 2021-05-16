/**
 * @file    colors.c
 * @brief   Receive two lines from the camera and uses mask to get the RGB values separately
 *
 * @author  David Niederhauser and Valentin Roch
 */

#include "hal.h"
#include <colors.h>
#include <main.h>
#include <camera/po8030.h>

#define 	LINE_SIZE				150
#define		THRESHOLD_RED			120
#define		THRESHOLD_RED_GREEN		16
#define		THRESHOLD_GREEN			21
#define		THRESHOLD_GREEN_RED		65
#define		THRESHOLD_BLUE			13
#define		THRESHOLD_BLUE_RED		70
#define 	THRESHOLD_COUNTER		4000

static int16_t	speed_r=0;
static int16_t	speed_l=0;
static bool color_detected=false;
static int16_t speed_reduction=0;
static uint16_t counter=0;

uint8_t get_colors(void)
{
static	uint8_t red[IMAGE_BUFFER_SIZE] = {0};
static	uint8_t green[IMAGE_BUFFER_SIZE] = {0};
static 	uint8_t blue[IMAGE_BUFFER_SIZE] = {0};
uint8_t *img_buff_ptr;
uint16_t counter_red=0;
uint16_t counter_green=0;
uint16_t counter_blue=0;

	img_buff_ptr = dcmi_get_last_image_ptr();

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
		{
			red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
			green[i/2] = (((uint8_t)img_buff_ptr[i+1]&0xE0)>>5) + (((uint8_t)img_buff_ptr[i]&0x07)<<3);
			blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;

			if(red[i/2]>THRESHOLD_RED && green[i/2]<THRESHOLD_RED_GREEN)
			{
				counter_red+=1;
			}
			if(green[i/2]>THRESHOLD_GREEN && red[i/2]<THRESHOLD_GREEN_RED)
			{
				counter_green+=1;
			}
			if(blue[i/2]>THRESHOLD_BLUE && red[i/2]<THRESHOLD_BLUE_RED)
			{
				counter_blue+=1;
			}
		}

	if(counter_red>LINE_SIZE)
	{
		color_detected=true;
		return RED;
	}
	else if(counter_green>LINE_SIZE)
	{
		counter+=1;

		//Counter is used to be sure that the robot sees some green
		if(counter>THRESHOLD_COUNTER)
		{
			color_detected=false;
			counter=0;
			return GREEN;
		}
	}
	else if(counter_blue>(LINE_SIZE))
	{
		return BLUE;
	}
	else
	{
		counter=0;
	}
	return NO_COLOR;
}

static THD_WORKING_AREA(waColorDetection, 2048);
static THD_FUNCTION(ColorDetection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;


    while(1)
    {
    	uint8_t color = get_colors();
    	static bool red=false;

    	if(color==RED)
    	{
    		red=true;

    		//Speed=0
    		speed_reduction=SPEED_EPUCK;
    	}
    	else if(color==GREEN)
		{
    		red=false;

    		//Speed=SPEED_EPUCK
    		speed_reduction=0;
		}
    	else if(color==BLUE && !red)
    	{
    		speed_reduction=SPEED_EPUCK/2;
    	}
    	else if(!red)
    	{
    		speed_reduction=0;
    	}

    	 chThdYield();
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

int16_t return_speed_reduction(void)
{
	return speed_reduction;
}

void color_detection_start(void)
{
	chThdCreateStatic(waColorDetection, sizeof(waColorDetection), NORMALPRIO, ColorDetection, NULL);
}
