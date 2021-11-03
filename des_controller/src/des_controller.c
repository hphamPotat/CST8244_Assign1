#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iomsg.h>
#include "des.h"
#include <fcntl.h>


//START_STATE = 0,
//IDLE_STATE = 1,
//FIRST_DOOR_SCAN_STATE = 2,
//GUARD_FIRST_DOOR_UNLOCK_STATE = 3,
//FIRST_DOOR_OPEN_STATE = 4,
//WEIGHT_SCANNED_STATE = 5,
//FIRST_DOOR_CLOSE_STATE = 6,
//GUARD_FIRST_DOOR_LOCK_STATE = 7,
//GUARD_SECOND_DOOR_UNLOCK_STATE = 8,
//SECOND_DOOR_OPEN_STATE = 9,
//SECOND_DOOR_CLOSE_STATE = 10,
//GUARD_SECOND_DOOR_LOCK_STATE = 11,
//EXIT_STATE = 12


// Function pointers declaration
void *startState();
void *idleState();
void *firstDoorScanState();
void *guardFirstDoorUnlockState();
void *firstDoorOpenState();
void *weightScanState();
void *firstDoorCloseState();
void *guardFirstDoorLockState();
void *guardSecondDoorUnlockState();
void *secondDoorOpenState();
void *secondDoorCloseState();
void *guardSecondDoorLockState();
void *exitState();

// Wut to do
void checkExitState();


Display display;
Person person;
//NextState nextState = startState;
NextState nextState = firstDoorScanState;
Direction direction = DEFAULT;
int controllerCoid;

int main(int argc, char* argv[]) {

	int rcvid;
	int controllerChid;
	pid_t displayPid;

	// Checking if Display's PID was passed in
	if (argc != 2){
		perror("Missing Display's PID\n");
		exit(EXIT_FAILURE);
	}
	displayPid = atoi(argv[1]);

	// Create channel for the inputs process to attach
	controllerChid = ChannelCreate(0);
	if (controllerChid == -1){
		perror("Failed to create channel\n");
		exit(EXIT_FAILURE);
	}

	// attach to display's PID
	controllerCoid = ConnectAttach(ND_LOCAL_NODE, displayPid, 1, _NTO_SIDE_CHANNEL, 0);


	// Print controller PID
	printf("The controller is running as PID: %d\nWaiting for Person...\n\n", getpid());


	// Loop to set next state
	while(1){
		rcvid = MsgReceive(controllerChid, &person, sizeof(Person), NULL);

		if (MsgReply(rcvid, EOK, &person, sizeof(Person)) == -1){
			perror("Controller failed to reply input\n");
			exit(EXIT_FAILURE);
		}

		if (strcmp(person.event, inMessage[EXIT]) == 0) {
			(*exitState)();
			break;
		}

		nextState = (NextState) (*nextState)();

		if (nextState == idleState){
			nextState = (NextState)(*nextState)();
		}
	}

	return EXIT_SUCCESS;
}


void checkExitState(){
	if (strcmp(person.event, inMessage[EXIT]) == 0){
		(*exitState)();
	}
}

void *startState(){
//	checkExitState();
	return idleState;

}

void *idleState(){
//	checkExitState();
	direction = DEFAULT;
	person.direction = DEFAULT;
	person.weight = -1;
	person.personId = -99;
//	person.state = IDLE_STATE;

	display.person = person;

	return firstDoorScanState;
}

/*
 * Function pointer for when the user first scans.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *firstDoorScanState(){
//	checkExitState();

	if ((strcmp(person.event, inMessage[RS]) == 0 || strcmp(person.event, inMessage[LS]) == 0) && direction == DEFAULT) {
		direction = person.direction;
		display.person = person;
		display.indexOutMessage = OUT_LS_RS;

		if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
			perror("Controller failed to send message (firstDoorScanState)\n");
			exit(EXIT_FAILURE);
		}

		return guardFirstDoorUnlockState;
	}

	return firstDoorScanState;
}


/*
 * Function pointer for when the guard unlocks the first door.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardFirstDoorUnlockState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[GLU]) == 0 && direction == LEFT){
		display.person = person;
		display.indexOutMessage = OUT_GLU;

	} else if (strcmp(person.event, inMessage[GRU]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_GRU;
	} else {
		return guardFirstDoorUnlockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardFirstDoorUnlockState)\n");
		exit(EXIT_FAILURE);
	}

	return firstDoorOpenState;
}


/*
 * Function pointer for when the first door is opened.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *firstDoorOpenState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[LO]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_LO;

	} else if (strcmp(person.event, inMessage[RO]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_RO;
	} else {
		return firstDoorOpenState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (firstDoorOpenState)\n");
		exit(EXIT_FAILURE);
	}

	return weightScanState;
}


/*
 * Function pointer for when the weight is scanned.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *weightScanState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[WS]) == 0) {
		display.person = person;
		display.indexOutMessage = OUT_WS;

		if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
			perror("Controller failed to send message (weightScanState)\n");
			exit(EXIT_FAILURE);
		}

		return firstDoorCloseState;
	}

	return weightScanState;
}


/*
 * Function pointer for when the first door is closed.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *firstDoorCloseState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[LC]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_LC;

	} else if (strcmp(person.event, inMessage[RC]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_RC;
	} else {
		return firstDoorCloseState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (firstDoorCloseState)\n");
		exit(EXIT_FAILURE);
	}

	return guardFirstDoorLockState;
}


/*
 * Function pointer for when the first door is locked.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardFirstDoorLockState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[GLL]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_GLL;

	} else if (strcmp(person.event, inMessage[GRL]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_GRL;
	} else {
		return guardFirstDoorLockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardFirstDoorLockState)\n");
		exit(EXIT_FAILURE);
	}

	return guardSecondDoorUnlockState;
}


/*
 * Function pointer for when the second door is unlocked.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardSecondDoorUnlockState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[GRU]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_GRU;

	} else if (strcmp(person.event, inMessage[GLU]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_GLU;
	} else {
		return guardSecondDoorUnlockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardSecondDoorUnlockState)\n");
		exit(EXIT_FAILURE);
	}

	return secondDoorOpenState;
}


/*
 * Function pointer for when the second door is opened.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *secondDoorOpenState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[RO]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_RO;

	} else if (strcmp(person.event, inMessage[LO]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_LO;
	} else {
		return secondDoorOpenState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (secondDoorOpenState)\n");
		exit(EXIT_FAILURE);
	}

	return secondDoorCloseState;
}


/*
 * Function pointer for when the second door is closed.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *secondDoorCloseState(){
//	checkExitState();

	if (strcmp(person.event, inMessage[RC]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_RC;

	} else if (strcmp(person.event, inMessage[LC]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_LC;
		printf("R-L second door close\n");
	} else {
		printf("In else\n");
		return secondDoorCloseState;
	}
	printf("B4 MsgSend\n");
	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (secondDoorCloseState)\n");
		exit(EXIT_FAILURE);
	}

	printf("After MsgSend\n");
	return guardSecondDoorLockState;
}


/*
 * Function pointer for when the second door is locked.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardSecondDoorLockState() {
//	checkExitState();

	if (strcmp(person.event, inMessage[GRL]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_GRL;

	} else if (strcmp(person.event, inMessage[GLL]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_GLL;
	} else {
		return guardSecondDoorLockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardSecondDoorLockState)\n");
		exit(EXIT_FAILURE);
	}

	return idleState;
}


/*
 * Function pointer for when the user entered in EXIT.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *exitState() {
	if (strcmp(person.event, inMessage[EXIT]) == 0) {
		display.indexOutMessage = OUT_EXIT;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardSecondDoorLockState)\n");
		exit(EXIT_FAILURE);
	}

	printf("Exiting controller\n");

	return exitState;
}
