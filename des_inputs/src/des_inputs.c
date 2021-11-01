#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iomsg.h>
#include "../../des_controller/src/des.h"
#include <fcntl.h>

int main(int argc, char* argv[]) {
	int coid;
	char userInput[25];	// Don't think need 50 characters for inputs with 4 characters max
	pid_t controllerPid;
	Person person;

	// Check if Controller's PID was passed in from the command line
	if (argc != 2) {
		perror("Missing Controller's PID\n");
		exit(EXIT_FAILURE);
	}

	// Assigning the controller PID to a variable
	controllerPid = atoi(argv[1]);

	// Attach to controller's channel
	coid = ConnectAttach(ND_LOCAL_NODE, controllerPid, 1, _NTO_SIDE_CHANNEL, 0);


	if (coid == -1){
		printf("Could not Connect Attach ControllerPid\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("Enter the event type (ls = left scan, rs = right scan, ws = weight scale, lo = left open, ro = right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard right lock, gll = guard left lock, glu = guard left unlock):\n");
		scanf("%s", &userInput);

		// = rs or ls, prompt person id, enter person id
		if (strcmp(userInput, inMessage[RS]) == 0 || strcmp(userInput, inMessage[LS]) == 0) {
			printf("Enter Person ID:\n");
			scanf("%d", &person.personId);
			person.direction = strcmp(userInput, inMessage[RS]) == 0 ? RIGHT : LEFT;
			person.state = FIRST_DOOR_SCAN_STATE;

		} else if (strcmp(userInput, inMessage[WS]) == 0) {
			// Prompt for the weight, enter weight
			printf("Enter Person weight: \n");
			scanf("%d", &person.weight);
			person.state = WEIGHT_SCANNED_STATE;

		}
//		else if (strcmp(userInput, inMessage[LO]) == 0) {
//			if (person.direction == LEFT)
//				person.state = FIRST_DOOR_OPEN_STATE;
//			else
//				person.state = SECOND_DOOR_OPEN_STATE;
//
//		} else if (strcmp(userInput, inMessage[RO]) == 0) {
//			if (person.direction == RIGHT)
//				person.state = FIRST_DOOR_OPEN_STATE;
//			else
//				person.state = SECOND_DOOR_OPEN_STATE;
//
//		} else if (strcmp(userInput, inMessage[LC]) == 0) {
//			if (person.direction == LEFT)
//				person.state = FIRST_DOOR_CLOSE_STATE;
//			else
//				person.state = SECOND_DOOR_CLOSE_STATE;
//
//		} else if (strcmp(userInput, inMessage[RC]) == 0) {
//			if (person.direction == RIGHT)
//				person.state = FIRST_DOOR_CLOSE_STATE;
//			else
//				person.state = SECOND_DOOR_CLOSE_STATE;
//
//		} else if (strcmp(userInput, inMessage[GRU]) == 0) {
//			if (person.direction == RIGHT)
//				person.state = GUARD_FIRST_DOOR_UNLOCK_STATE;
//			else
//				person.state = GUARD_SECOND_DOOR_UNLOCK_STATE;
//
//		} else if (strcmp(userInput, inMessage[GRL]) == 0) {
//			if (person.direction == RIGHT)
//				person.state = GUARD_FIRST_DOOR_LOCK_STATE;
//			else
//				person.state = GUARD_SECOND_DOOR_LOCK_STATE;
//
//		} else if (strcmp(userInput, inMessage[GLL]) == 0) {
//			if (person.direction == LEFT)
//				person.state = GUARD_FIRST_DOOR_LOCK_STATE;
//			else
//				person.state = GUARD_SECOND_DOOR_LOCK_STATE;
//
//		} else if (strcmp(userInput, inMessage[GLU]) == 0) {
//			if (person.direction == LEFT)
//				person.state = GUARD_FIRST_DOOR_UNLOCK_STATE;
//			else
//				person.state = GUARD_SECOND_DOOR_UNLOCK_STATE;
//
//		} else if (strcmp(userInput, inMessage[EXIT]) == 0) {
//			person.state = EXIT_STATE;
//
//		} else {
//			printf("Invalid input");
//		}

		// no further prompt is required
//		else if (strcmp(userInput, "lo") == 0 || strcmp(userInput, "ro") == 0
//				|| strcmp(userInput, "lc") == 0 || strcmp(userInput, "rc") == 0
//				|| strcmp(userInput, "glu") == 0
//				|| strcmp(userInput, "gll") == 0
//				|| strcmp(userInput, "gru") == 0
//				|| strcmp(userInput, "grl") == 0) {
//			continue;	// Check if continue usage is correct?
//		}
		strcpy(person.event, userInput);

		// Send Message
		if (MsgSend(coid, &person, sizeof(Person), NULL, 0) == -1){ // What to put here for arguments
			perror("Failed to send message\n");
			exit(EXIT_FAILURE);
		}

		// if input is exit, end process
		if (strcmp(userInput, inMessage[EXIT]) == 0) {
			break;
		}
	}



	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
