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
	char userInput[50];	// Don't think need 50 characters for inputs with 4 characters max
	pid_t controllerPid;
	Person person;

	// Check if Controller's PID was passed in from the command line
	if (argc != 1) {
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
		printf(
				"Enter the event type (ls = left scan, rs = right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard right lock, gll=guard left lock, glu = guard left unlock)): ");
		scanf("%s", userInput);


		// = rs or ls, prompt person id, enter person id
		if (strcmp(userInput, "rs") == 0 || strcmp(userInput, "ls") == 0) {
			printf("Enter Person ID: ");
			scanf("%d", &person.personId);
		}
		// no further prompt is required
		else if (strcmp(userInput, "lo") == 0 || strcmp(userInput, "ro") == 0
				|| strcmp(userInput, "lc") == 0 || strcmp(userInput, "rc") == 0
				|| strcmp(userInput, "glu") == 0
				|| strcmp(userInput, "gll") == 0
				|| strcmp(userInput, "gru") == 0
				|| strcmp(userInput, "grl") == 0) {
			continue;	// Check if continue usage is correct?
		}
		// Prompt for the weight, enter weight
		else if (strcmp(userInput, "ws") == 0) {
			printf("Enter Person weight: ");
			scanf("%d", &person.weight);
		}
		else if (strcmp(userInput, "exit") == 0) {
			break;
		}
	}

	if (MsgSend(coid, &person, sizeof(Person)) == -1){ // What to put here for arguments
		perror("Failed to send message");
		exit(EXIT_FAILURE);
	}

	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
