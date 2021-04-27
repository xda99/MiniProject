#ifndef FOLLOW_LINE_H
#define FOLLOW_LINE_H

#define THRESHOLD_CURVE 	290 //number of pixels that define the beginning of a curve
//A GARDER dans le .h??


//start the thread to follow a line
void line_follow_start(void);
void virage(uint16_t position, bool right);

#endif /* FOLLOW_LINE_H */
