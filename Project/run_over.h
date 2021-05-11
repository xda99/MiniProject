#ifndef RUN_OVER_H
#define RUN_OVER_H

 /**
 * @brief   Start the skirt thread
 */
void skirt_start(void);

/**
* @brief 	Rotate in the best sens when there is an obstacle in front of the epuck
* 			and turn a bool to true when the obstacle is on his side
*/
void go_along(void);


/**
* @brief	Set the speed of the motors to make the epuck turn on the spot
*/
void rotation(int16_t direction);

/**
* @brief	Choose the best rotation sens when there is an obstacle in front of the epuck
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
* @brief	Return the status of the bool obstacle to know in other threads that there is an obstacle
* @return	The bool obstacle
*/
bool return_obstacle(void);

int16_t return_speed_l_ro(void);
int16_t return_speed_r_ro(void);
#endif /* RUN_OVER_H */
