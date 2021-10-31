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


Display display;

int main(int argc, char* argv[]) {
	int controllerCoid;
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
		rcvid = MsgReceive(controllerCoid, &person, sizeof(Person), NULL);
//		if (MsgReply(rcvid, EOK, &person, sizeof(Person)) == -1){
//
//		}
	}

	return EXIT_SUCCESS;
}


void *startState(){

}

void *idleState(){

}

void *firstDoorScanState(){
	if (display.person.state == EXIT_STATE){
		// return?
	}
	else if (display.person.direction == LEFT){
		display.person.state = LS;
	}
	else if (display.person.direction == RIGHT){
		display.person.state = RS;
	}
}

void *guardFirstDoorUnlockState(){

}

void *firstDoorOpenState(){

}

void *weightScanState(){

}

void *firstDoorCloseState(){

}

void *guardFirstDoorLockState(){

}

void *guardSecondDoorUnlockState(){

}

void *secondDoorOpenState(){

}

void *secondDoorCloseState(){

}

void *guardSecondDoorLockState(){

}

void *exitState(){

}
