#include "standalone.h"

/* Error helper functions */

// Error on malloc
void malloc_error() {
	printf("Failed to malloc\n");
	exit(-1);
}
// Error on getaddrinfo()
void error_gai(int error) {
	printf("error on getaddrinfo: %d\n", error);
	exit(-1);
}
// Error on opening socket
void error_sock() {
	printf("failed to open socket\n");
	exit(-1);
}
// General error with errno set
void error(int error) {
	printf("fatal error: %d\n", error);
	exit(-1);
}
// General error with message
void error_detail(char *error) {
	printf("error: %s\n", error);
	exit(-1);
}
// json parser error
void jsonError(const char *error, cJSON *json) {
	if (error != NULL) {
			printf("error: %s\n", error);
	}
	cJSON_Delete(json);
	exit(-1);
}
