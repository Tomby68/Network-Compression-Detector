#include "compdetection.h"


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: ./compdetect_server port_num\n");
		exit(-1);
	}

	char buf[1024];
	init_tcp_server(argv[1], buf);
	printf("buf = %s\n", buf);
}
