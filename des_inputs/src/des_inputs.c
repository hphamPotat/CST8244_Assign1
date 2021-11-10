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
#include <sys/dispatch.h>

int main(int argc, char* argv[]) {
	int coid;
	char userInput[25];	// Don't think need 50 characters for inputs with 4 characters max
//	pid_t controllerPid;
	Person person;


	// Check if Controller's PID was passed in from the command line
//	if (argc != 2) {
//		perror("Missing Controller's PID\n");
//		exit(EXIT_FAILURE);
//	}

	// Assigning the controller PID to a variable
//	controllerPid = atoi(argv[1]);

	// Attach to controller's channel
//	coid = ConnectAttach(ND_LOCAL_NODE, controllerPid, 1, _NTO_SIDE_CHANNEL, 0);
	coid = name_open("controllerName", 0);

	// Check if the connectAttach was successful or not
	if (coid == -1){
		printf("Could not Connect Attach ControllerPid\n");
		exit(EXIT_FAILURE);
	}

	// Loop to ask for user's input and actions
	while (1) {
		printf("Enter the event type (ls = left scan, rs = right scan, "
				"ws = weight scale, lo = left open, ro = right open, "
				"lc = left closed, rc = right closed , gru = guard right unlock, "
				"grl = guard right lock, gll = guard left lock, glu = guard left unlock):\n");
		scanf("%s", &userInput);

		// = rs or ls, prompt person id, enter person id
		if (strcmp(userInput, inMessage[RS]) == 0 || strcmp(userInput, inMessage[LS]) == 0) {
			printf("Enter Person's ID:\n");
			scanf("%d", &person.personId);
			person.direction = strcmp(userInput, inMessage[RS]) == 0 ? RIGHT : LEFT;
			person.state = DOOR_SCAN_STATE;

		}
		// Prompt for the weight, enter weight
		else if (strcmp(userInput, inMessage[WS]) == 0) {
			printf("Enter the Person's weight: \n");
			scanf("%d", &person.weight);
			person.state = WEIGHT_SCAN_STATE;

		}

		// Copy userInput into event variable in person struct so it can be accessed outside of the file
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



//	ConnectDetach(coid);
	name_close(coid);
	return EXIT_SUCCESS;
}
