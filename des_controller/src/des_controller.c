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


int main(int argc, char* argv[]) {
	int controllerCoid;
	int rcvid;
	int controllerChid;
	pid_t displayPid;
	Person person;

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
