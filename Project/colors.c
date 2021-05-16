/**
 * @file    colors.c
 * @brief   Receive two lines from the camera and uses mask to get the RGB values separately
 *
 * @author  David Niederhauser and Valentin Roch
 */

#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>
#include <motors.h>
#include <run_over.h>

#include <process_image.h>

#include <pal.h>

#include <colors.h>

#define 	LINE_SIZE				100//175//40
#define		THRESHOLD_RED			120//130//140
#define		THRESHOLD_GREEN			21//22//24//22//27
#define		THRESHOLD_BLUE			13//16

static int16_t	speed_r=0;
static int16_t	speed_l=0;
static bool color_detected=false;
static int16_t speed_reduction=0;
static uint16_t compteur=0;

uint8_t get_colors(void)
{
static	uint8_t red[IMAGE_BUFFER_SIZE] = {0};
static	uint8_t green[IMAGE_BUFFER_SIZE] = {0};
static 	uint8_t blue[IMAGE_BUFFER_SIZE] = {0};
uint8_t *img_buff_ptr;
uint16_t compt_red=0;
uint16_t compt_green=0;
uint16_t compt_blue=0;

	img_buff_ptr = dcmi_get_last_image_ptr();

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
		{
			red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
			green[i/2] = (((uint8_t)img_buff_ptr[i+1]&0xE0)>>5) + (((uint8_t)img_buff_ptr[i]&0x07)<<3);
			blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;

			if(red[i/2]>THRESHOLD_RED && green[i/2]<16)
			{
				compt_red+=1;
			}
			if(green[i/2]>THRESHOLD_GREEN && red[i/2]<65)
			{
				compt_green+=1;
			}
			if(blue[i/2]>THRESHOLD_BLUE && red[i/2]<70) //62
			{
				compt_blue+=1;
			}
		}

	if(compt_red>LINE_SIZE-10)
	{
		color_detected=true;
		chprintf((BaseSequentialStream *)&SD3,"red\n");
		return RED;
	}
	else if(compt_green>LINE_SIZE+20)
	{
		compteur+=1;

		if(compteur>4000)
		{
			color_detected=false;
			compteur=0;
			chprintf((BaseSequentialStream *)&SD3,"green\n");
			return GREEN;
		}
	}
	else if(compt_blue>(LINE_SIZE-50))
	{
		return BLUE;
	}
	else
	{
		compteur=0;
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
    	else if(color==GREEN)
		{
			speed_r=SPEED_EPUCK;
			speed_l=SPEED_EPUCK;
		}
    	else if(color==BLUE)
    	{
    		speed_reduction=SPEED_EPUCK/2;
    	}
    	else
    	{
    		speed_reduction=0;
    	}

//    	chThdSleepMilliseconds(10);
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

void color_detection_start(void)
{
	chThdCreateStatic(waColorDetection, sizeof(waColorDetection), NORMALPRIO, ColorDetection, NULL);
}

bool return_color_detected(void)
{
	return color_detected;
}

int16_t return_speed_reduction(void)
{
	return speed_reduction;
}
