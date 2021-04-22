#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>

#include <pal.h>

#include <colors.h>

#define 	LINE_SIZE	50

#define 	RED			1
#define		GREEN		2
#define		BLUE		3
#define		BLACK		4

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

	img_buff_ptr = dcmi_get_last_image_ptr();

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
		{
			red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;

			if(red[i/2]>110)
			{
				compt_red+=1;
			}
			if(red[i/2]<10)
			{
				compt_black+=1;
			}

			green[i/2] = ((uint8_t)img_buff_ptr[i+1]&0xE0>>5) + ((uint8_t)img_buff_ptr[i]&0x07<<3);
			if(green[i/2]>22)
			{
				compt_green+=1;
			}

			blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;
			if(blue[i/2]>12)
			{
				compt_blue+=1;
			}
		}

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
/*
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
