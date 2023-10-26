#include "compdetection.h"

void init_tcp_client(struct config_details config, char *file_contents) {
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(config.server_ip, config.port_tcp_pre, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}

	err_check = connect(fd, res->ai_addr, res->ai_addrlen);
	if (err_check == -1) {
		error(errno);
	}

	int sent = send(fd, file_contents, strlen(file_contents), 0);
	if (sent < (int) strlen(file_contents)) {
		error_detail("failed to send");
	}

	close(fd);
	freeaddrinfo(res);
}

void init_tcp_server(char *port_num, char *buf) {
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

	socklen_t addr_size = sizeof(client_addr);
	int sfd = accept(fd, (struct sockaddr *) &client_addr, &addr_size);
	if (sfd == -1) {
		error_sock();
	}
	close(fd);
	freeaddrinfo(res);

	int rec = recv(sfd, buf, MAX_CONFIG_SIZE, 0);
	if (rec == -1) {
		error(rec);
	} else if (rec == 0) {
		printf("Connection closed by foreign host\n");
	}

	buf[rec] = '\0';
}
