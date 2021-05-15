/**
 * @file    process_image.h
 * @brief	Receives an images from the camera and detect the position of the black line
 *
 *			Note that this file was taken from TP4_CamReg_correction and was modified
 *
 * @author  David Niederhauser, Valentin Roch and TP4_CamReg_correction
 */

#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

/**
* @brief   	Extracts the line width of the line detected from the buffer parameter
*
* @param	Buffer containing the processed image
*
* @return	Returns the line's width extracted from the image buffer given
*  			Returns 0 if line not found
*/
uint16_t extract_line_width(uint8_t *buffer);

/**
* @return	Returns the position of the middle of the line detected
*  			Returns IMAGE_BUFFER_SIZE/2 if no line is detected
*/
uint16_t get_line_position(void);

/**
* @return	The variable lineWidth to use it in another file
*/
uint16_t get_line_width(void);

/**
* @return	The variable line_not_found to use it in another file
*/
uint8_t get_line_not_found(void);

/**
* @brief   	Begins the thread ProcessImage
*/
void process_image_start(void);

#endif /* PROCESS_IMAGE_H */
