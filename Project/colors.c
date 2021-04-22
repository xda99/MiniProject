#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <process_image.h>

#include <pal.h>

#include <colors.h>

#define LINE_SIZE	200

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



bool red(void)
{
	uint8_t red[IMAGE_BUFFER_SIZE] = {0};
	uint8_t *img_buff_ptr;

	img_buff_ptr = dcmi_get_last_image_ptr();
	//Extract only the red pixels
	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
	{
		red[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
	}
	return color_detection(red);
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
		green[i/2] = ((uint8_t)img_buff_ptr[i+1]&0xE0) + ((uint8_t)img_buff_ptr[i]&07);
		if(green[i/2]>250)
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

}

bool blue(void)
{
	uint8_t blue[IMAGE_BUFFER_SIZE] = {0};
	uint8_t *img_buff_ptr;

	img_buff_ptr = dcmi_get_last_image_ptr();

	for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
	{
		blue[i/2] = (uint8_t)img_buff_ptr[i+1]&0x1F;
	}
	return color_detection(blue);
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
}
