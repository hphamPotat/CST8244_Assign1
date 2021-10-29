/*
 * des-mva.h
 *
 *  An example H file that you can use as a model for defining enums for the inputs, outputs, and state names.
 *  This approach will make your code more readable :)
 */

#ifndef DOOR_ENTRY_SYSTEM_H_
#define DOOR_ENTRY_SYSTEM_H_

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

	//TODO :: to see the list of //TODOS, Window > Show View > Tasks
	//TODO :: assign an enum value, one for each state
} State;

#define NUM_INPUTS 12
typedef enum
{
//	LEFT_BUTTON_DOWN = 0,
//	LEFT_BUTTON_UP = 1,
//	RIGHT_BUTTON_DOWN = 2,
//	RIGHT_BUTTON_UP = 3,
//	STOP_BUTTON = 4

	LS = 0,
	RS = 1,
	WS = 2,
	LO = 3,
	RO = 4,
	LC = 5,
	RC = 6,
	GRU = 7,
	GRL = 8,
	GLL = 9,
	GLU = 10,
	EXIT = 11

	//TODO :: assign an enum value, one for each input command
} Input;

const char *inMessage[NUM_INPUTS] = {
	"ls",	// left scan
	"rs",	// right scan
	"ws",	// weight scale
	"lo",	// left open
	"ro",	// right open
	"lc",	// left closed
	"rc",	// right closed
	"gru",	// guard right unlock
	"grl",	// guard right lock
	"gll",	// guard left lock
	"glu",	// guard left unlock
	"exit"	// exit
};


#define NUM_OUTPUTS 11
typedef enum
{
//	START_MSG = 0,
//	READY_MSG = 1,
//	LEFT_DOWN_MSG = 2,
//	RIGHT_DOWN_MSG = 3,
//	ARMED_MSG = 4,
//	PUNCH_MSG = 5,
//	EXIT_MSG = 6,
//	STOP_MSG = 7

	OUT_LS_RS = 0,
	OUT_WS = 1,
	OUT_LO = 2,
	OUT_RO = 3,
	OUT_LC = 4,
	OUT_RC = 5,
	OUT_GRU = 6,
	OUT_GRL = 7,
	OUT_GLL = 8,
	OUT_GLU = 9,
	OUT_EXIT = 10
	//TODO :: assign an enum value, one for each output message from the FSM
} Output;


//TODO :: each output message. For example, "Person opened left door"
const char *outMessage[NUM_OUTPUTS] = {
//	"Start Message",
//	"Ready.",
//	"Left Button Down = Press Right Button to Arm Press",
//	"Right Button Down - Press Left Button to Arm Press",
//	"DANGER - Press Armed",
//	"Punching.",
//	"Exiting.",
//	"Stop Message."

	"Person scanned ID. ID = ",
	"Person weighed. Weight = ",
	"Person opened left door",
	"Person opened right door",
	"Person closed left door",
	"Person closed right door",
	"Right door unlocked by Guard",
	"Right door locked by Guard",
	"Left door locked by Guard",
	"Left door unlocked by Guard"
	"Exit Display"
};

// inputs client sends a Person struct to its server, the controller
typedef struct {
	//TODO :: fields for person's ID (int), weight (int), direction (inbound or outbound),
	//	and some way to remember what state the Person is in. Suppose the Person in "Left Scan" state.
	//	You need a way to represent that.
	int personId;
	int weight;
	int state;
	int direction;
} Person;

// controller client sends a Display struct to its server, the display
typedef struct {
	//TODO :: fields for the output message and the Person. For the output message, I used an int, which
	//	is the index into the outMessages array. That way I don't pass strings in my messages.
	//	The Person field is needed, as some output message require information from the Person.
	//	Specifically, those messages that display the Person's ID and weight.

	char errMessage[50]; // check again
	int indexOutMessage;
	Person person;
} Display;


#endif /* DES_MVA_H_ */
