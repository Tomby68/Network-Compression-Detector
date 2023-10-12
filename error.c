#include "compdetection.h"

void error_gai(int error) {
	printf("error on getaddrinfo: %d\n", error);
	exit(-1);
}

void error(int error) {
	printf("fatal error: %d\n", error);
	exit(-1);
}
