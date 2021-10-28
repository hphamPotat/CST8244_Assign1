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

		if (display.indexOutMessage == OUT_LS_RS){
			printf("%s %d\n", outMessage[OUT_LS_RS], display.person.personId);
		}
		else if (display.indexOutMessage == OUT_WS){
			printf("%s %d\n", outMessage[OUT_WS], display.person.weight);
		}
		else {
			int index = display.indexOutMessage;

			if (index >= 2 && index <= 10){
				printf("%s\n", outMessage[index]);
			}
		}
//		else if (display.person.state == OUT_LO){
//			printf("%s\n", outMessage[OUT_LO]);
//		}
//		else if (display.person.state == OUT_RO){
//			printf("%s\n", outMessage[OUT_RO]);
//		}
//		else if (display.person.state == OUT_LC){
//			printf("%s\n", outMessage[OUT_LC]);
//		}
//		else if (display.person.state == OUT_RC){
//			printf("%s\n", outMessage[OUT_RC]);
//		}
//		else if (display.person.state == OUT_GRU){
//			printf("%s\n", outMessage[OUT_GRU]);
//		}
//		else if (display.person.state == OUT_GRL){
//			printf("%s\n", outMessage[OUT_GRL]);
//		}
//		else if (display.person.state == OUT_GLL){
//			printf("%s\n", outMessage[OUT_GLL]);
//		}
//		else if (display.person.state == OUT_GLU){
//			printf("%s\n", outMessage[OUT_GLU]);
//		}
//		else if (display.person.state == OUT_EXIT){
//			printf("%s\n");
//		}
	}


	ChannelDestroy(chid);

	return EXIT_SUCCESS;
}
