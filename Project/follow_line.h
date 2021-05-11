#ifndef FOLLOW_LINE_H
#define FOLLOW_LINE_H


#define THRESHOLD_CURVE 	290 //number of pixels/threshold that define the beginning of a curve


/////////////////////////////////////////////////////////////////////////7///////////////////////////////A GARDER dans le .h?????????????????
/////////////////////////////////////////////////////////////////////////////////////////////////////////////à/////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @brief  The robot travels the given distance
*
* @param distance in mm
* 		 speed in step/s
*/
void position(float distance, int16_t speed);
//start the thread to follow a line


/**
* @brief  The epuck detects the direction of a turn and follows the line in the curve
*/
void curve(void);

/**
* @brief  The epuck follows a straight line when there is no curves
*/
void straight_line(int16_t speed_correction);

/**
* @brief  The epuck follows a straight line when there is no curves
*/
void color_detection(uint8_t color);
//void angle_rotation(float angle, int16_t speed_r);
//void position(float distance, int16_t speed);

/**
* @brief  Begins the thread LineFollow
*/
void line_follow_start(void);

int16_t return_speed_l_fl(void);
int16_t return_speed_r_fl(void);

#endif /* FOLLOW_LINE_H */
