#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iomsg.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	int coid;
	pid_t controllerPid;
	char* userInput;

	if (argc != 1){
		perror("Missing PID to display\n");
		exit(EXIT_FAILURE);
	}

	coid = ConnectAttach(ND_LOCAL_NODE, controllerPid, 1, _NTO_SIDE_CHANNEL, 0);

	while(1){
		printf("Enter DES Input-event: ");
		scanf("%s",userInput);

		if (strcmp(userInput, "ls") == 0){

		}
		else if (strcmp(userInput, "rs")){

		}
		else if (strcmp(userInput, "ws")){

		}

		if (strcmp(userInput, "exit")){
			break;
		}
	}


	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
