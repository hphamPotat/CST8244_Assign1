/*
 * des-mva.h
 *
 *  An example H file that you can use as a model for defining enums for the inputs, outputs, and state names.
 *  This approach will make your code more readable :)
 */

#ifndef DES_H_
#define DES_H_

#define NUM_STATES 8
typedef enum
{
	START_STATE = 0,
	READY_STATE = 1,
	LEFT_DOWN_STATE = 2,
	RIGHT_DOWN_STATE = 3,
	ARMED_STATE = 4,
	PUNCH_STATE = 5,
	EXIT_STATE = 6,
	STOP_STATE = 7
} State;

#define NUM_INPUTS 5
typedef enum
{
	LEFT_BUTTON_DOWN = 0,
	LEFT_BUTTON_UP = 1,
	RIGHT_BUTTON_DOWN = 2,
	RIGHT_BUTTON_UP = 3,
	STOP_BUTTON = 4
} Input;

#define NUM_OUTPUTS 8
typedef enum
{
	START_MSG = 0,
	READY_MSG = 1,
	LEFT_DOWN_MSG = 2,
	RIGHT_DOWN_MSG = 3,
	ARMED_MSG = 4,
	PUNCH_MSG = 5,
	EXIT_MSG = 6,
	STOP_MSG = 7
} Output;

const char *outMessage[NUM_OUTPUTS] = {
	"Start Message",
	"Ready.",
	"Left Button Down = Press Right Button to Arm Press",
	"Right Button Down - Press Left Button to Arm Press",
	"DANGER - Press Armed",
	"Punching.",
	"Exiting.",
	"Stop Message."};

const char *inMessage[NUM_INPUTS] = {
	"LD",
	"LU",
	"RD",
	"RU",
	"S"};
#endif /* DES_H_ */
