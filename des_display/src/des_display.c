#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "../../des_controller/src/des.h"
#include <sys/neutrino.h>
#include <sys/dispatch.h>

int main(void) {
	int rcvid;
	Display display;
	name_attach_t *displayAttach;

	// Create Channel for controller process to attach
//	int displayChid = ChannelCreate(0);
	displayAttach = name_attach(NULL, "displayName", 0);

	// Check if channel creation was successful or not
//	if (displayChid == -1) {
//		perror("Failed to create a channel\n");
//		exit(EXIT_FAILURE);
//	}
	if (displayAttach == NULL) {
		perror("Failed to create a channel\n");
		exit(EXIT_FAILURE);
	}

	// Print Display PID
	printf("The display is running as PID: %d\n\n", getpid());

	// Loop to display the right messages to the console corresponding to action event
	while (1) {
//		rcvid = MsgReceive(displayChid, &display, sizeof(Display), NULL);
		rcvid = MsgReceive(displayAttach->chid, &display, sizeof(Display), NULL);

		if (rcvid == -1) {
			perror("Failed to receive message\n");
			exit(EXIT_FAILURE);
		}

		// If the action was LS or RS
		if (display.indexOutMessage == OUT_LS_RS) {
			printf("%s %d\n", outMessage[OUT_LS_RS], display.person.personId);
		}
		// If the action was WS
		else if (display.indexOutMessage == OUT_WS) {
			printf("%s %d\n", outMessage[OUT_WS], display.person.weight);
		}
		// Others
		else
			printf("%s\n", outMessage[display.indexOutMessage]);


		// Check if message was replied/printed successfully
		if (MsgReply(rcvid, EOK, &display, sizeof(display)) == -1) {
			perror("Failed to send message\n");
			exit(EXIT_FAILURE);
		}

		// If the action was EXIT
		if (display.indexOutMessage == OUT_EXIT)
			break;
	}

	// Destroy channel
//	ChannelDestroy(displayChid);
	name_detach(displayAttach, 0);

	return EXIT_SUCCESS;
}
