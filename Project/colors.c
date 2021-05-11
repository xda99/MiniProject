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

#define 	LINE_SIZE				40
#define		THRESHOLD_RED			140
#define		THRESHOLD_GREEN			27
#define		THRESHOLD_BLUE			16
#define 	THRESHOLD_BLACK			10
#define 	THRESHOLD_YELLOW_R		80
#define 	THRESHOLD_YELLOW_G	 	17


/*
void color(void)
{
	uint8_t red[IMAGE_BUFFER_SIZE] = {0};
	uint8_t green[IMAGE_BUFFER_SIZE] = {0};
	uint8_t blue[IMAGE_BUFFER_SIZE] = {0};

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
	{
		red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
		green[i/2] = ((uint8_t)img_buff_ptr[i+1]&0xE0) + ((uint8_t)img_buff_ptr[i]&07);
		blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;
	}
}
*/

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
	uint16_t compt_yellow=0;

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

			if(green[i/2]>THRESHOLD_GREEN)
			{
				compt_green+=1;
			}

			blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;

			if(blue[i/2]>THRESHOLD_BLUE)
			{
				compt_blue+=1;
			}
			if(red[i/2]>THRESHOLD_YELLOW_R && green[i/2]>THRESHOLD_YELLOW_G)
			{
				compt_yellow+=1;
			}
		}

//	SendUint8ToComputer(green, IMAGE_BUFFER_SIZE);

	if(compt_red>LINE_SIZE)
	{
		return RED;
	}
	else if(compt_green>LINE_SIZE)
	{
		return GREEN;
	}
	else if(compt_blue>LINE_SIZE)
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
	}
	else
	{
		return NO_COLOR;
	}
}

static THD_WORKING_AREA(waColorDetection, 2048);
static THD_FUNCTION(ColorDetection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	systime_t time;

    while(1)
    {
    	time = chVTGetSystemTime();
    	uint8_t color = get_colors();

    	if(color==RED)
    	{
    		//chprintf((BaseSequentialStream *)&SD3,"red\n");
    		right_motor_set_speed(0);
    		left_motor_set_speed(0);
    	}
    	if(color==GREEN)
		{
			//chprintf((BaseSequentialStream *)&SD3,"green\n");
			right_motor_set_speed(SPEED_EPUCK);
			left_motor_set_speed(SPEED_EPUCK);
		}
/*    	if(color==BLUE)
    	{
    		//chprintf((BaseSequentialStream *)&SD3,"blue\n");
    	}
      	if(color==BLACK)
    	{
    		chprintf((BaseSequentialStream *)&SD3,"black\n");
    	}
     	if(color==YELLOW) // Passage pieton => Ralenti
		{
			chprintf((BaseSequentialStream *)&SD3,"yellow\n");
    		position=right_motor_get_pos();
			right_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			left_motor_set_pos(CAMERA__DISTANCE_CORRECTION);
			do{
				right_motor_set_speed(SPEED_EPUCK-100);
				left_motor_set_speed(SPEED_EPUCK-100);
			}while(abs(position-right_motor_get_pos())<CAMERA__DISTANCE_CORRECTION);
		}*/

    	 //100Hz
    	 chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void color_detection_start(void)
{
	chThdCreateStatic(waColorDetection, sizeof(waColorDetection), NORMALPRIO+1, ColorDetection, NULL);
}


/*
bool red(void)
{
	uint8_t red[IMAGE_BUFFER_SIZE] = {0};
	uint8_t *img_buff_ptr;
	uint16_t compt=0;
	uint16_t compt_black=0;

	img_buff_ptr = dcmi_get_last_image_ptr();
	//Extract only the red pixels
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
	{
		red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
		if(red[i/2]>110)
		{
			compt+=1;
		}
		if(red[i/2]<10)
		{
			compt_black+=1;
		}
	}

	//SendUint8ToComputer(red, IMAGE_BUFFER_SIZE);
	if(compt>LINE_SIZE)
	{
		return true;
	}
	else
	{
		return false;
	}
	//return color_detection(red);
}

bool green(void)
{
	uint8_t green[IMAGE_BUFFER_SIZE] = {0};
	uint8_t *img_buff_ptr;

	uint16_t compt=0;

	img_buff_ptr = dcmi_get_last_image_ptr();

	//Extract only the green pixels
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
	{
		green[i/2] = ((uint8_t)img_buff_ptr[i+1]&0xE0>>5) + ((uint8_t)img_buff_ptr[i]&0x07<<3);

		if(green[i/2]>22)
		{
			compt+=1;
		}
	}

	SendUint8ToComputer(green, IMAGE_BUFFER_SIZE);

	if(compt>LINE_SIZE)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool blue(void)
{
	uint8_t blue[IMAGE_BUFFER_SIZE] = {0};
	uint8_t *img_buff_ptr;
	uint16_t compt=0;

	img_buff_ptr = dcmi_get_last_image_ptr();

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
	{
		blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;

		if(blue[i/2]>12)
		{
			compt+=1;
		}
	}
	//SendUint8ToComputer(blue, IMAGE_BUFFER_SIZE);

	if(compt>LINE_SIZE)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool color_detection(uint8_t color[IMAGE_BUFFER_SIZE])
{
	uint16_t compt=0;
	for(uint16_t i=0; i<IMAGE_BUFFER_SIZE; i+=1)
	{
		if(color[i]>120)
		{
			compt+=1;
		}
	}
	if(compt>LINE_SIZE)
	{
		return true;
	}
	else
	{
		return false;
	}
}*/
