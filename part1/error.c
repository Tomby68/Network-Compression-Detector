#include "compdetection.h"

void malloc_error() {
	printf("Failed to malloc\n");
	exit(-1);
}

void error_gai(int error) {
	printf("error on getaddrinfo: %d\n", error);
	exit(-1);
}

void error_sock() {
	printf("failed to open socket\n");
	exit(-1);
}

void error(int error) {
	printf("fatal error: %d\n", error);
	exit(-1);
}

void error_detail(char *error) {
	printf("error: %s\n", error);
	exit(-1);
}

void jsonError(const char *error, cJSON *json) {
	if (error != NULL) {
			printf("error: %s\n", error);
	}
	cJSON_Delete(json);
	exit(-1);
}
