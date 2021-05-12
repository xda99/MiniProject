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
* @brief   	Return a speed for the left motor in function of the color seen
*
* @return	The static int16_t speed_l
*/
int16_t return_speed_l_c(void);

/**
* @brief   	Return a speed for the right motor in function of the color seen
*
* @return	The static int16_t speed_r
*/
int16_t return_speed_r_c(void);

/**
* @brief	Return the status of the bool color_detected to know in other threads that there is a specified color in front of the camera
*
* @return	The static bool color_detected
*/
bool return_color_detected(void);

#endif /* COLORS_H */
