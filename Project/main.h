/**
 * @file    main.h
 * @brief   initializes the sensors and the motors and starts the threads
 *
 * @author  David Niederhauser and Valentin Roch
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE				640
#define WIDTH_SLOPE						5
#define MIN_LINE_WIDTH					200
#define ROTATION_THRESHOLD				10
#define ROTATION_COEFF					2
#define PXTOCM							1570.0f //experimental value
#define GOAL_DISTANCE 					10.0f
#define MAX_DISTANCE 					25.0f
#define ERROR_THRESHOLD					10
#define KP								4
#define KI 								3.5f//must not be zero
#define KD								1
#define MAX_SUM_ERROR 					(MOTOR_SPEED_LIMIT/KI)
#define SPEED_EPUCK						250 //constant speed of the epuck in step/s
#define CAMERA__DISTANCE_CORRECTION		450 //correction as the camera is not under the e-puck
#define IR_VALUE						120
#define WHEEL_DISTANCE					53 //mm
#define LINE_FOUND						2
/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
