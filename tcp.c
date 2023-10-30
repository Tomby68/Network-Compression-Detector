#include "compdetection.h"

void tcp_send(struct config_details config, char *to_send, char *to_send2, char *ip_addr) {
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	char *port_num = config.port_tcp_pre;
	if (strlen(to_send2) > 0) {
		port_num = config.port_tcp_post;
	}
	int err_check = getaddrinfo(ip_addr, port_num, &hints, &res);
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
	
	freeaddrinfo(res);
	
	int sent = send(fd, to_send, strlen(to_send), 0);
	if (sent < (int) strlen(to_send)) {
		error_detail("failed to send msg 1");
	}
	if (strlen(to_send2) > 0) {
		sent = send(fd, to_send2, strlen(to_send2), 0);
		if (sent < (int) strlen(to_send2)) {
			error_detail("failed to send msg 2");
		}
	}

	close(fd); 
}

struct sockaddr tcp_recv(char *port_num, char *buf1, char *buf2) {
	struct addrinfo hints;
	struct sockaddr client_ip;
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

	socklen_t *client_ip_len = (uint32_t *) sizeof(client_ip);
	err_check = getpeername(fd, &client_ip, client_ip_len);
	if (err_check) {
		error(errno);
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

	int rec = recv(sfd, buf1, MAX_CONFIG_SIZE, 0);
	if (rec == -1) {
		error(rec);
	} else if (rec == 0) {
		printf("Connection closed by foreign host\n");
	}

	if (sizeof(buf2) > sizeof(char *)) {
		rec = recv(sfd, buf2, MAX_CONFIG_SIZE, 0);
		if (rec == -1) {
			error(rec);
		} else if (rec == 0) {
			printf("Connection closed by foreign host\n");
		}
	} 

	buf1[rec] = '\0';
	return client_ip;
}
