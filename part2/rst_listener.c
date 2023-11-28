#include "standalone.h"

void *rst_listen(void *fd) {
	char buffer[4096];
	struct timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;
	int err_check = setsockopt(*(int *) fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (err_check == -1) {
		error(errno);
	}
	int rec = read(*(int *) fd, buffer, 4096);
	if (rec > 0) {
		printf("Caught: %s\n", buffer);
	} else {
		printf("Failed!\n");
	}
	return 0;
}
