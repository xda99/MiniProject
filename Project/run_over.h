/**
 * @file    run_over.h
 * @brief	Detects if there is an obstacle in front of the epuck and go along it if so
 *
 *	 As a reminder for the numbers of the IR sensors from the library e-puck2_main-processor/src/sensors/proximity.c
 *   - IR0 (front-right) + IR4 (back-left)
 *   - IR1 (front-right-45deg) + IR5 (left)
 *   - IR2 (right) + IR6 (front-left-45deg)
 *   - IR3 (back-right) + IR7 (front-left)
 *
 * @author  David Niederhauser and Valentin Roch
 */
#ifndef RUN_OVER_H
#define RUN_OVER_H

/**
* @brief 	Rotates to the best side when there is an obstacle in front of the epuck
* 			and turns a bool to true when the obstacle is on its side
*/
void go_along(void);

/**
* @brief Regulates the distance between the epuck and the obstacle while skirting it
*
* @return A correction speed to keep the obstacle on the epuck's side
*/
int16_t regulator(void);

/**
* @brief	Sets the speed of the motors to make the epuck turn on the spot
*/
void rotation(int16_t direction);

/**
* @brief	Chooses the best rotation side when there is an obstacle in front of the epuck
*
* @return	True if the epuck will turn to the left
*/
bool turn_left(void);

/**
* @brief   	Returns a speed for the left motor in function of the color seen
*
* @return	The static int16_t speed_l
*/
int16_t return_speed_l_ro(void);

/**
* @brief   	Returns a speed for the right motor in function of the color seen
*
* @return	The static int16_t speed_r
*/
int16_t return_speed_r_ro(void);

/**
* @brief	Returns the status of the bool obstacle to know in other threads that there is an obstacle
* @return	The static bool obstacle
*/
bool return_obstacle(void);

/**
* @brief   Starts the skirt thread
*/
void skirt_start(void);

#endif /* RUN_OVER_H */
