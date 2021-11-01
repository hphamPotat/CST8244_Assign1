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
void checkExitState();


Display display;
Person person;
NextState nextState = startState;
Direction direction = DEFAULT;
int controllerCoid;

int main(int argc, char* argv[]) {

	int rcvid;
	int controllerChid;
	pid_t displayPid;

	// Checking if Display's PID was passed in
	if (argc != 1){
		perror("Missing Display's PID");
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


	printf("Controller PID: %d\n", getpid());


	while(1){
		rcvid = MsgReceive(controllerChid, &person, sizeof(Person), NULL);
//		if (MsgReply(rcvid, EOK, &person, sizeof(Person)) == -1){
//
//		}

		nextState = (NextState)(*nextState)();
	}

	return EXIT_SUCCESS;
}


void checkExitState(){
	if (strcmp(person.event,inMessage[EXIT]) == 0){
		exitState;
	}
}

void *startState(){
	checkExitState();
	return idleState;

}

void *idleState(){
	checkExitState();

	person.state = IDLE_STATE;
	return firstDoorScanState;

}

void *firstDoorScanState(){
	checkExitState();

	if ((strcmp(person.event, inMessage[RS]) == 0 || strcmp(person.event, inMessage[LS]) == 0) && direction == DEFAULT) {
		direction = person.direction;
		display.person = person;
		display.indexOutMessage = OUT_LS_RS;

		if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
			perror("Controller failed to send message (firstDoorScanState)");
			exit(EXIT_FAILURE);
		}

		return guardFirstDoorUnlockState;
	}

	return firstDoorScanState;
}

void *guardFirstDoorUnlockState(){
	checkExitState();

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
		perror("Controller failed to send message (guardFirstDoorUnlockState)");
		exit(EXIT_FAILURE);
	}

	return firstDoorOpenState;
}

void *firstDoorOpenState(){
	checkExitState();

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
		perror("Controller failed to send message (firstDoorOpenState)");
		exit(EXIT_FAILURE);
	}

	return weightScanState;
}

void *weightScanState(){
	checkExitState();

	if (strcmp(person.event, inMessage[WS]) == 0) {
		display.person = person;
		display.indexOutMessage = OUT_WS;

		if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
			perror("Controller failed to send message (weightScanState)");
			exit(EXIT_FAILURE);
		}

		return firstDoorCloseState;
	}

	return weightScanState;
}

void *firstDoorCloseState(){
	checkExitState();

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
		perror("Controller failed to send message (firstDoorCloseState)");
		exit(EXIT_FAILURE);
	}

	return guardFirstDoorLockState;
}

void *guardFirstDoorLockState(){
	checkExitState();

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
		perror("Controller failed to send message (guardFirstDoorLockState)");
		exit(EXIT_FAILURE);
	}

	return guardSecondDoorUnlockState;
}

void *guardSecondDoorUnlockState(){
	checkExitState();

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
		perror("Controller failed to send message (guardSecondDoorUnlockState)");
		exit(EXIT_FAILURE);
	}

	return secondDoorOpenState;
}

void *secondDoorOpenState(){
	checkExitState();

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
		perror("Controller failed to send message (secondDoorOpenState)");
		exit(EXIT_FAILURE);
	}

	return secondDoorCloseState;
}

void *secondDoorCloseState(){
	checkExitState();

	if (strcmp(person.event, inMessage[RC]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_RC;

	} else if (strcmp(person.event, inMessage[LC]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_LC;
	} else {
		return secondDoorCloseState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (secondDoorCloseState)");
		exit(EXIT_FAILURE);
	}

	return guardSecondDoorLockState;
}

void *guardSecondDoorLockState(){
	checkExitState();

	if (strcmp(person.event, inMessage[GRL]) == 0 && direction == LEFT) {
		display.person = person;
		display.indexOutMessage = OUT_GRL;

	} else if (strcmp(person.event, inMessage[GRU]) == 0 && direction == RIGHT) {
		display.person = person;
		display.indexOutMessage = OUT_GRU;
	} else {
		return guardSecondDoorLockState;
	}

	if (MsgSend(controllerCoid, &display, sizeof(Display), &display, sizeof(Display)) == -1L) {
		perror("Controller failed to send message (guardSecondDoorLockState)");
		exit(EXIT_FAILURE);
	}

	return idleState;
}

void *exitState(){

}
