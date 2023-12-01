#include "standalone.h"

/* Listen for any packets coming in on the raw socket - If a packet is received, check
 * if is an RST packet in response to the head or tail SYN packet. If so, record the timestamp.
 * Updates args with the difference in head and tail RST timestamps
 * If both RST packets are not received within 60 seconds, the program assumes they were lost and times out
 *
 * args:
 * void *args: The arg_struct to update
 * 		Should contain: A raw socket file descriptor, a head_port and tail_port 
 * 		to verify the RST packets, and a difference value to fill in
 */
void *rst_listen(void *a) {
	// Retreive arg_struct from void * input
	struct arg_struct *arguments;
	arguments = (struct arg_struct*) a;
	
	// Initialize struct, addr_len, and buffer, which will all be filled in by the recvfrom() call
	struct sockaddr_storage server;
	socklen_t addr_len = sizeof(server);
	char buffer[4096];
	
	printf("Listening for RST packets...\n");
	// Begin the first timer to check for timeout
	struct timeval timer1;
	gettimeofday(&timer1, NULL);
	
	// Initialize the timestamp variables for the RST packets
	struct timespec first_rst;
	struct timespec second_rst;

	// Parse the head_port and tail_port from arguments -> Stored in 16 byte shorts to make later comparisons easier
	unsigned short int head_port = ((atoi(arguments->head_port) >> 8) << 8) | (atoi(arguments->head_port) & 0xff);
	unsigned short int tail_port = ((atoi(arguments->tail_port) >> 8) << 8) | (atoi(arguments->tail_port) & 0xff);

	// Set a 15 second timeout for the socket so it will check if the overall timeout has been reached every 15 seconds
	struct timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;
	int err_check = setsockopt(arguments->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (err_check == -1) {
		printf("Failed to set socket timeout\n");
		error(errno);
	}

	// Initialize a second timer to compare with timer1 -> Checks if timeout has been reached
	struct timeval timer2;
	gettimeofday(&timer2, NULL);
	int head_recved = 0;
	int tail_recved = 0;
	struct timespec tmp_timer;

	// Continously read from the socket and check if the proper RST packets have been received
	// Continue while:
	// timeout has not been reached
	// AND
	// The head or tail RST packets have not been received
	while ((timer2.tv_sec - timer1.tv_sec < 60) && (!head_recved || !tail_recved)) {
		int rec = recvfrom(arguments->fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *) &server, &addr_len);
		// If something was received over the socket, check: first, if it is an RST packet
		// and second: If that RST packet corresponds to the ports that the head and tail packets were sent to
		if (rec > 0) {
			int time_call = clock_gettime(CLOCK_REALTIME, &tmp_timer);
			if (time_call != 0) {
				printf("Error: Failed to get time\n");
				error(errno);
			}
			if (buffer[33] == 20) {
				// Check bytes 20/21, which make up the source port
				unsigned short int sport = ((buffer[20] & 0xff) << 8) | (buffer[21] &0xff);
				// Check if the rst packet is from the head or tail SYN packet based on the port
				if (sport == head_port) {
					first_rst = tmp_timer;
					head_recved = 1;
				} else if (sport == tail_port) {
					second_rst = tmp_timer;
					tail_recved = 1;
				}
			}
		} 
		gettimeofday(&timer2, NULL);
	}
	// Update arguments with the RST timestamp difference
	if (timer2.tv_sec - timer1.tv_sec >= 60) {
		arguments->difference = 0;
	} else {
		printf("RST packets found\n");
		arguments->difference = ((second_rst.tv_sec - first_rst.tv_sec) * SEC_TO_MS) + ((second_rst.tv_nsec - first_rst.tv_nsec) / NS_TO_MS);
	}
	return 0;
}
