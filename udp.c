#include "compdetection.h"

void init_udp_client(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(config.server_ip, config.dest_port_udp, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}

	int bytes_sent = sendto(fd, config.udp_payload, strlen(config.udp_payload), 0, res->ai_addr, res->ai_addrlen);
	printf("sent: %s\nlength: %lu\n", config.udp_payload, strlen(config.udp_payload));
	if (bytes_sent == -1) {
		error(errno);
	}
	freeaddrinfo(res);
	close(fd);
}	

void init_udp_server(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage client;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(NULL, config.source_port_udp, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}

	err_check = bind(fd, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	char buf[1024];
	socklen_t addr_len = sizeof(client);
	int bytes_recv = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client, &addr_len);
	if (bytes_recv == -1) {
		error(bytes_recv);
	} else if (bytes_recv == 0) {
		printf("Connection closed by foreign host\n");
	}

	buf[bytes_recv] = '\0';
	printf("recved over udp: buf = %s\n", buf);
}
