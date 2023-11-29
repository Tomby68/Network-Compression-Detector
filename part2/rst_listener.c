#include "standalone.h"

void *rst_listen(void *args) {
	struct arg_struct *arguments;
	arguments = (struct arg_struct*) args;
	printf("Listening for RST packets...\n");
	struct timeval timer1;
	gettimeofday(&timer1, NULL);
	long first_rst = -1;
	long second_rst = -1;
	
	struct sockaddr_storage server;
	socklen_t addr_len = sizeof(server);

	char buffer[4096];
	struct timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;
	int err_check = setsockopt(arguments->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (err_check == -1) {
		printf("Failed to set socket timeout\n");
		error(errno);
	}
	struct timeval timer2;
	gettimeofday(&timer2, NULL);
	int head_recved = 0;
	int tail_recved = 0;
	while ((timer2.tv_sec - timer1.tv_sec < 60) & (!head_recved | !tail_recved)) {
		int rec = recvfrom(arguments->fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *) &server, &addr_len);
		if (rec > 0) {
			printf("Caught!\n");
			if (buffer[33] == 20) {
				printf("Caught RST packet\n");
				// Check bytes 20/21, which make up the source port
				char *sport = &buffer[20];
				strlcat(sport, &buffer[21], sizeof(sport));
				// Check if the rst packet is from the head or tail SYN packet based on the port
				if (!strcmp(sport, arguments->head_port)) {
					first_rst = clock();
					head_recved = 1;
				}
				if (!strcmp(sport, arguments->tail_port)) {
					second_rst = clock();
					tail_recved = 1;
				}
			}
		} else {
			printf("Failed!\n");
		}
		gettimeofday(&timer2, NULL);
	}
	
	return (void *) (second_rst - first_rst);
}
