#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/neutrino.h>

int main(void) {
	int rcvid;
	// Create Channel
	int chid = ChannelCreate(0);

	// Check if channel creation was successful or not
	if (chid == -1){
		perror("Failed to create a channel\n");
		exit(EXIT_FAILURE);
	}

	// Print current PID
	printf("PID: %d\n",getpid());

	while(1){
		rcvid = MsgReceive(chid, );
		MsgReply(rcvid, EOK, );


	}


	ChannelDestroy(chid);

	return EXIT_SUCCESS;
}
