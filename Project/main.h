/**
 * @file    main.h
 * @brief   Initializes the sensors and the motors and starts the threads
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

#define IMAGE_BUFFER_SIZE				640
#define SPEED_EPUCK						250 //constant speed of the epuck in step/s
#define LINE_FOUND						2

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
