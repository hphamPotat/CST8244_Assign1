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
#include <sys/dispatch.h>


// Function pointers declaration
void *startIdleState();
void *doorScanState();
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

/*
 * Display struct to be sent to des_display for output.
 */
Display display;
/*
 * Person struct for receiving data from des_inputs.
 */
Person person;
/*
 * Tracks the next state funciton pointer to be call.
 */
NextState nextState = startIdleState;
/*
 * The connection ID for passing messages to des_display.
 */
int controllerCoid;


int main(int argc, char* argv[]) {
	/*
	 * The message receive ID for replying to des_inputs.
	 */
	int rcvid;
	/*
	 * The controller's channel ID
	 */
//	int controllerChid;
	name_attach_t *controllerAttach;

	/*
	 * The process ID of the display.
	 */
//	pid_t displayPid;


	// Checking if Display's PID was passed in
//	if (argc != 2){
//		perror("Missing Display's PID\n");
//		exit(EXIT_FAILURE);
//	}
//	displayPid = atoi(argv[1]);

	// Create channel for the inputs process to attach
//	controllerChid = ChannelCreate(0);
//	if (controllerChid == -1){
//		perror("Failed to create channel\n");
//		exit(EXIT_FAILURE);
//	}
	controllerAttach = name_attach(NULL,"controllerName",0);



	// attach to display's PID
//	controllerCoid = ConnectAttach(ND_LOCAL_NODE, displayPid, 1, _NTO_SIDE_CHANNEL, 0);
	controllerCoid = name_open("displayName", 0);


	// Print controller PID
	printf("The controller is running as PID: %d\nWaiting for Person...\n\n", getpid());

	// Loop to set next state
	while(1){
		if (nextState == startIdleState){
			nextState = (NextState)(*nextState)();
		}

//		rcvid = MsgReceive(controllerChid, &person, sizeof(Person), NULL);
		rcvid = MsgReceive(controllerAttach->chid, &person, sizeof(Person), NULL);

		if (strcmp(person.event, inMessage[EXIT]) == 0) {
			(*exitState)();
		} else nextState = (NextState) (*nextState)();

		if (MsgReply(rcvid, EOK, &person, sizeof(Person)) == -1){
			perror("Controller failed to reply input\n");
			exit(EXIT_FAILURE);
		}

		if (strcmp(person.event, inMessage[EXIT]) == 0) {
			break;
		}
	}

	// Detach from des_display
//	ConnectDetach(controllerCoid);
	// Destroy channel
//	ChannelDestroy(controllerChid);

	name_detach(controllerAttach, 0);
	name_close(controllerCoid);

	return EXIT_SUCCESS;
}


/*
 * Function pointer for the start/idle state.
 * Runs when controller is first started, and after a person goes through the des.
 *
 * Resets all of the values, for the next person to go through the des.
 */
void *startIdleState(){
	display.person.direction = DEFAULT;
	display.person.weight = -1;
	display.person.personId = -99;
	display.person.state = DOOR_SCAN_STATE;

	return doorScanState;
}

/*
 * Function pointer for when the user first scans.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *doorScanState(){
	if (display.person.state != DOOR_SCAN_STATE){
		return doorScanState;
	}

	if ((strcmp(person.event, inMessage[RS]) == 0 || strcmp(person.event, inMessage[LS]) == 0) && display.person.direction == DEFAULT) {
		display.person = person;
		display.indexOutMessage = OUT_LS_RS;

		if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
			perror("Controller failed to send message (doorScanState)\n");
			exit(EXIT_FAILURE);
		}

		display.person.state = GUARD_FIRST_DOOR_UNLOCK_STATE;
		return guardFirstDoorUnlockState;
	}

	return doorScanState;
}


/*
 * Function pointer for when the guard unlocks the first door.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardFirstDoorUnlockState(){
	if (display.person.state != GUARD_FIRST_DOOR_UNLOCK_STATE){
		return guardFirstDoorUnlockState;
	}

	if (strcmp(person.event, inMessage[GLU]) == 0 && display.person.direction == LEFT){
		strcpy(display.person.event, person.event);
//		display.person.event = person.event;
		display.indexOutMessage = OUT_GLU;

	} else if (strcmp(person.event, inMessage[GRU]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GRU;
	} else {
		return guardFirstDoorUnlockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardFirstDoorUnlockState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = FIRST_DOOR_OPEN_STATE;
	return firstDoorOpenState;
}


/*
 * Function pointer for when the first door is opened.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *firstDoorOpenState(){
	if (display.person.state != FIRST_DOOR_OPEN_STATE){
		return firstDoorOpenState;
	}

	if (strcmp(person.event, inMessage[LO]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_LO;

	} else if (strcmp(person.event, inMessage[RO]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_RO;
	} else {
		return firstDoorOpenState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (firstDoorOpenState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = WEIGHT_SCAN_STATE;
	return weightScanState;
}


/*
 * Function pointer for when the weight is scanned.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *weightScanState(){
	if (display.person.state != WEIGHT_SCAN_STATE){
		return weightScanState;
	}

	if (strcmp(person.event, inMessage[WS]) == 0) {
		strcpy(display.person.event, person.event);
		display.person.weight = person.weight;
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_WS;

		if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
			perror("Controller failed to send message (weightScanState)\n");
			exit(EXIT_FAILURE);
		}

		display.person.state = FIRST_DOOR_CLOSE_STATE;
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
	if (display.person.state != FIRST_DOOR_CLOSE_STATE){
		return firstDoorCloseState;
	}

	if (strcmp(person.event, inMessage[LC]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_LC;

	} else if (strcmp(person.event, inMessage[RC]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_RC;
	} else {
		return firstDoorCloseState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (firstDoorCloseState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = GUARD_FIRST_DOOR_LOCK_STATE;
	return guardFirstDoorLockState;
}


/*
 * Function pointer for when the first door is locked.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardFirstDoorLockState(){
	if (display.person.state != GUARD_FIRST_DOOR_LOCK_STATE){
		return guardFirstDoorLockState;
	}

	if (strcmp(person.event, inMessage[GLL]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GLL;

	} else if (strcmp(person.event, inMessage[GRL]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GRL;
	} else {
		return guardFirstDoorLockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardFirstDoorLockState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = GUARD_SECOND_DOOR_UNLOCK_STATE;
	return guardSecondDoorUnlockState;
}


/*
 * Function pointer for when the second door is unlocked.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardSecondDoorUnlockState(){
	if (display.person.state != GUARD_SECOND_DOOR_UNLOCK_STATE){
		return guardSecondDoorUnlockState;
	}

	if (strcmp(person.event, inMessage[GRU]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GRU;

	} else if (strcmp(person.event, inMessage[GLU]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GLU;
	} else {
		return guardSecondDoorUnlockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardSecondDoorUnlockState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = SECOND_DOOR_OPEN_STATE;
	return secondDoorOpenState;
}


/*
 * Function pointer for when the second door is opened.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *secondDoorOpenState(){
	if (display.person.state != SECOND_DOOR_OPEN_STATE){
		return secondDoorOpenState;
	}

	if (strcmp(person.event, inMessage[RO]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_RO;

	} else if (strcmp(person.event, inMessage[LO]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_LO;
	} else {
		return secondDoorOpenState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (secondDoorOpenState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = SECOND_DOOR_CLOSE_STATE;
	return secondDoorCloseState;
}


/*
 * Function pointer for when the second door is closed.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *secondDoorCloseState(){
	if (display.person.state != SECOND_DOOR_CLOSE_STATE){
		return secondDoorCloseState;
	}

	if (strcmp(person.event, inMessage[RC]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_RC;

	} else if (strcmp(person.event, inMessage[LC]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_LC;
	} else {
		return secondDoorCloseState;
	}
	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (secondDoorCloseState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = GUARD_SECOND_DOOR_LOCK_STATE;
	return guardSecondDoorLockState;
}


/*
 * Function pointer for when the second door is locked.
 * Set according print out statements to the console
 *
 * If fail to send message, point back to the function itself
 */
void *guardSecondDoorLockState() {
	if (display.person.state != GUARD_SECOND_DOOR_LOCK_STATE){
		return guardSecondDoorLockState;
	}

	if (strcmp(person.event, inMessage[GRL]) == 0 && display.person.direction == LEFT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GRL;

	} else if (strcmp(person.event, inMessage[GLL]) == 0 && display.person.direction == RIGHT) {
		strcpy(display.person.event, person.event);
		//		display.person.event = person.event;
		display.indexOutMessage = OUT_GLL;
	} else {
		return guardSecondDoorLockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardSecondDoorLockState)\n");
		exit(EXIT_FAILURE);
	}

	display.person.state = START_IDLE_STATE;
	return startIdleState;
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
