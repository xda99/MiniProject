#ifndef COLORS_H
#define COLORS_H

//Get the values of the camera with a mask to get only the red
//Return true if there is some red
//bool red(void);

//Get the values of the camera with a mask to get only the green
//Return true if there is some green
//bool green(void);

//Get the values of the camera with a mask to get only the blue
//Return true if there is some blue
//bool blue(void);

//Detect if there is a significant amount of pixels with the color entered in parameter
//Return true if so
//bool color_detection(uint8_t color[IMAGE_BUFFER_SIZE]);


#define 	RED			1
#define		GREEN		2
#define		BLUE		3
#define		BLACK		4
#define		YELLOW		5
#define		NO_COLOR	6

/**
* @brief   Detects which color is present in front of the epuck
*
* @return					The color detected using a define
*/

uint8_t get_colors(void);

/**
* @brief   Begins the thread LineFollow
*/

void color_detection_start(void);


#endif /* COLORS_H */
