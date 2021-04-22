#ifndef COLORS_H
#define COLORS_H

//Get the values of the camera with a mask to get only the red
//Return true if there is some red
bool red(void);

//Get the values of the camera with a mask to get only the green
//Return true if there is some green
bool green(void);

//Get the values of the camera with a mask to get only the blue
//Return true if there is some blue
bool blue(void);

//Detect if there is a significant amount of pixels with the color entered in parameter
//Return true if so
//bool color_detection(uint8_t color[IMAGE_BUFFER_SIZE]);

#endif /* COLORS_H */
