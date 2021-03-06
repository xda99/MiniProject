/**
 * @file    follow_line.h
 * @brief   Everything that keep the robot in the center of the line
 *
 * @author  David Niederhauser and Valentin Roch
 */
#ifndef FOLLOW_LINE_H
#define FOLLOW_LINE_H

/**
* @brief  	The robot travels the given distance
*
* @param 	distance in mm
* 		 	speed in step/s
*/
void position(float distance, int16_t speed);

/**
* @brief  The epuck detects the direction of a curve and follows the line in the curve
*/
void curve(void);

/**
* @brief  The epuck follows and stay aligned with a straight line when there is no curve
*/
void straight_line(int16_t speed_correction);

/**
* @brief   	Returns a speed for the left motor in function of the color seen
*
* @return	The static int16_t speed_l
*/
int16_t return_speed_l_fl(void);

/**
* @brief   	Returns a speed for the right motor in function of the color seen
*
* @return	The static int16_t speed_r
*/
int16_t return_speed_r_fl(void);

/**
* @brief  Starts the thread LineFollow
*/
void line_follow_start(void);

#endif /* FOLLOW_LINE_H */
