/**
 * @file    move.h
 * @brief	Receives from the files "follow_line", "colors" and "run_over" the speed to give to both motors
 * 			and sets it
 *
 *
 * @author  David Niederhauser and Valentin Roch
 */
#ifndef MOVE_H
#define MOVE_H

/**
* @brief	Sets the speed of the motors in function of the black line, the obstacles and the colors detected
*/
void move(void);

#endif /* MOVE_H */
