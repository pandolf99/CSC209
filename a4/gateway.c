#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>
#include "socket.h"
#include "message.h"
#include "controller.h"

#define MAXFD(x,y) ((x) >= (y)) ? (x) : (y)

int main(int argc, char *argv[]){
	int port;
	struct cignal cig;
	// A buffer to store a serialized message
	char *cig_serialized = malloc(sizeof(char)*CIGLEN);
	// An array to registered sensor devices
	int device_record[MAXDEV] = {0};

	if(argc == 2){
		port = strtol(argv[1], NULL, 0);
	} else{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}

	int gatewayfd = set_up_server_socket(port);
	printf("\nThe Cignal Gateway is now started on port: %d\n\n", port);
	int peerfd;

	/* TODO: Implement the body of the server.
	 *
	 * Use select so that the server process never blocks on any call except
	 * select. If no sensors connect and/or send messsages in a timespan of
	 * 5 seconds then select will return and print the message "Waiting for
	 * Sensors update..." and go back to waiting for an event.
	 *
	 * The server will handle connections from devices, will read a message from
	 * a sensor, process the message (using process_message), write back
	 * a response message to the sensor client, and close the connection.
	 * After reading a message, your program must print the "RAW MESSAGE"
	 * message below, which shows the serialized message received from the *
	 * client.
	 *
	 *  Print statements you must use:
     * 	printf("Waiting for Sensors update...\n");
	 * 	printf("RAW MESSAGE: %s\n", YOUR_VARIABLE);
	 */

	// TODO implement select loop

	// Suppress unused variable warning.  The next 5 ilnes can be removed
	// after the variables are used.
	(void)gatewayfd;
	(void)peerfd;
	(void)cig;
	(void)device_record;
	(void)cig_serialized;

	int max_fd = gatewayfd;
	fd_set all_fds;
	FD_ZERO(&all_fds);
	FD_SET(gatewayfd, &all_fds);

	while(1) {
		// Set up inputs for select.
		fd_set listen_fds = all_fds;
		struct timeval wait_time;
    wait_time.tv_sec = (time_t) 5;
    wait_time.tv_usec= 0;
		int nready = select(max_fd + 1, &listen_fds, NULL, NULL, &wait_time);

    if (nready == -1) {
        perror("server: select");
        exit(1);
    }
		//If nothing happens.
		if (nready  == 0) {
			printf("Waiting for Sensors update...\n");
		}

    // Accept new connection.
    if (FD_ISSET(gatewayfd, &listen_fds)) {
        int peerfd = accept_connection(gatewayfd);
        if (peerfd > max_fd) {
            max_fd = peerfd;
        }
				if (peerfd > 0) {
					FD_SET(peerfd, &all_fds);
				}
				else {
					exit(-1);
				}
    }

		//Iterate over clients.
		for( int i = 0; i <= max_fd; i++) {
			if (i == gatewayfd) continue;
			if (FD_ISSET(i, &listen_fds)) {
				if (read(i, cig_serialized, sizeof(char)*CIGLEN) < 0){
					perror("Server: Read:");
					exit(-1);
				}
				printf("RAW MESSAGE: %s\n", cig_serialized);
				unpack_cignal(cig_serialized, &cig);
				//If it succesfully updated, update message type if necessary.
				int success = process_message(&cig, device_record);
				if (success == 2) {
					cig.hdr.type = 3;
				}
				if (success < 0) {
					fprintf(stderr, "Could not process a message\n");
					continue;
				}
				//create new string with updated cig to write to client.
				//And copy it to cig_serialized.
				//Note that serialize_cignal() calls Malloc so necessary to free.
				char *cig_serialized_temp = serialize_cignal(cig);
				if (strncpy(cig_serialized, cig_serialized_temp, sizeof(char)*CIGLEN) == NULL){
					perror("Server: copying updated message:");
					exit(-1);
				}
				free(cig_serialized_temp);

				if (write(i, cig_serialized, sizeof(char)*CIGLEN) < 0) {
					perror("Server: Write:");
					exit(-1);
				}
				//Clean up
				FD_CLR(i, &all_fds);
				close(i);
			}
		}
	}
	return 0;
}
