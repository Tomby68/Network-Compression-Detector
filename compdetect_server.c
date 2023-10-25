#include "compdetection.h"

#define NLOG 5


void initialize_tcp(char *port_num) {
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage client_addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(NULL, port_num, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error_sock();
	}

	err_check = bind(fd, res->ai_addr, res->ai_addrlen);
	if (err_check == -1) {
		error(err_check);
	}

	err_check = listen(fd, NLOG);
	if (err_check == -1) {
		error(err_check);
	}
	//accept()

	socklen_t addr_size = sizeof(client_addr);
	int sfd = accept(fd, (struct sockaddr *) &client_addr, &addr_size);
	if (sfd == -1) {
		error_sock();
	}
	close(fd);
	freeaddrinfo(res);

	char buf[1024];
	int rec = recv(sfd, buf, sizeof(buf), 0);
	if (rec == -1) {
		error(rec);
	} else if (rec == 0) {
		printf("Connection closed by foreign host\n");
	}
}


int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Usage: ./compdetect_server port_num\n");
		exit(-1);
	}

	initialize_tcp(argv[1]);
}
