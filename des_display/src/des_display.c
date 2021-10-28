#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "../../des_controller/src/des.h"
#include <sys/neutrino.h>

int main(void) {
	int rcvid;
	Display display;

	// Create Channel for controller process to attach
	int displayChid = ChannelCreate(0);

	// Check if channel creation was successful or not
	if (displayChid == -1){
		perror("Failed to create a channel\n");
		exit(EXIT_FAILURE);
	}

	// Print Display PID
	printf("Display PID: %d\n",getpid());

	while(1){
		rcvid = MsgReceive(displayChid, display, sizeof(Display),NULL);

		if (rcvid == -1){

		}

		if (MsgReply(rcvid, EOK, &display, sizeof(display)) == -1){ // Should this be moved to bottom of while loop?

		}

		// should ID_scan be a const somewhere in the .h file
	}


	ChannelDestroy(chid);

	return EXIT_SUCCESS;
}
