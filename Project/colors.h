/**
 * @file    colors.h
 * @brief   Receives two lines from the camera and uses masks to get the RGB values separately
 * 			The colors affect the speed of the robot
 *
 * @author  David Niederhauser and Valentin Roch
 */

#ifndef COLORS_H
#define COLORS_H

/**
* @brief   	Detects which color is present in front of the epuck
*
* @return	The color detected using a define
*/
uint8_t get_colors(void);

/**
* @brief	Returns the speed reduction factor to go slower when there is blue
*
* @return	The static int16_t speed_reduction
*/
int16_t return_speed_reduction(void);

/**
* @brief   Begins the thread LineFollow
*/
void color_detection_start(void);

#endif /* COLORS_H */
