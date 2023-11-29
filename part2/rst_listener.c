#include "standalone.h"

void *rst_listen(void *fd) {
	printf("Listening for RST packets...\n");
	struct timeval timer1;
	gettimeofday(&timer1, NULL);

	struct sockaddr_storage server;
	socklen_t addr_len = sizeof(server);

	char buffer[4096];
	struct timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;
	int err_check = setsockopt(*(int *) fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (err_check == -1) {
		error(errno);
	}
	struct timeval timer2;
	gettimeofday(&timer2, NULL);
	printf("In listener: timer1 = %li, timer2 = %li\n", timer1.tv_sec, timer2.tv_sec);
	while ((timer2.tv_sec - timer1.tv_sec) < 60) {
		int rec = recvfrom(*(int *) fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *) &server, &addr_len);
		if (rec > 0) {
			printf("Caught: %s\n", buffer);
		} else {
			printf("Failed!\n");
		}
		gettimeofday(&timer2, NULL);
		printf("In listener: timer1 = %li, timer2 = %li\n", timer1.tv_sec, timer2.tv_sec);

	}
	return 0;
}
