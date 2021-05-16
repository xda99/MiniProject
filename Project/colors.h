/**
 * @file    colors.h
 * @brief   Receive two lines from the camera and uses mask to get the RGB values separately
 *
 * @author  David Niederhauser and Valentin Roch
 */

#ifndef COLORS_H
#define COLORS_H

#define 	RED			1
#define		GREEN		2
#define		BLUE		3
#define		BLACK		4
#define		YELLOW		5
#define		NO_COLOR	6

/**
* @brief   	Detects which color is present in front of the epuck
*
* @return	The color detected using a define
*/
uint8_t get_colors(void);

/**
* @brief   Begins the thread LineFollow
*/
void color_detection_start(void);

/**
* @brief   	Returns a speed for the left motor in function of the color seen
*
* @return	The static int16_t speed_l
*/
int16_t return_speed_l_c(void);

/**
* @brief   	Returns a speed for the right motor in function of the color seen
*
* @return	The static int16_t speed_r
*/
int16_t return_speed_r_c(void);

/**
* @brief	Returns the speed reduction factor to go slower when there is blue
*
* @return	The static int16_t speed_reduction
*/
int16_t return_speed_reduction(void);
#endif /* COLORS_H */
