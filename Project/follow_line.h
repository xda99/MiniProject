#ifndef FOLLOW_LINE_H
#define FOLLOW_LINE_H

//number of pixels/threshold that define the beginning of a curve
#define THRESHOLD_CURVE 	290 //////////////////////////////////////////////////////////////////////

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
* @brief   	Return a speed for the left motor in function of the color seen
*
* @return	The static int16_t speed_l
*/
int16_t return_speed_l_fl(void);

/**
* @brief   	Return a speed for the right motor in function of the color seen
*
* @return	The static int16_t speed_r
*/
int16_t return_speed_r_fl(void);

/**
* @brief  Start the thread LineFollow
*/
void line_follow_start(void);

#endif /* FOLLOW_LINE_H */
