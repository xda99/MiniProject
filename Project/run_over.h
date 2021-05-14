#ifndef RUN_OVER_H
#define RUN_OVER_H

 /**
 * @brief   Start the skirt thread
 */
void skirt_start(void);

/**
* @brief 	Rotate to the best side when there is an obstacle in front of the epuck
* 			and turn a bool to true when the obstacle is on his side
*/
void go_along(void);


/**
* @brief	Set the speed of the motors to make the epuck turn on the spot
*/
void rotation(int16_t direction);

/**
* @brief	Choose the best rotation side when there is an obstacle in front of the epuck
*
* @return	True if the epuck will turn to the left
*/
bool turn_left(void);

/**
* @brief Regulates the distance between the epuck and the obstacle while skirting it
*
* @return A correction speed to keep the obstacle on the epuck's side
*/
int16_t regulator(void);

/**
* @brief   	Return a speed for the left motor in function of the color seen
*
* @return	The static int16_t speed_l
*/
int16_t return_speed_l_ro(void);

/**
* @brief   	Return a speed for the right motor in function of the color seen
*
* @return	The static int16_t speed_r
*/
int16_t return_speed_r_ro(void);

/**
* @brief	Return the status of the bool obstacle to know in other threads that there is an obstacle
* @return	The static bool obstacle
*/
bool return_obstacle(void);

#endif /* RUN_OVER_H */
