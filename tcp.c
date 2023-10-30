#include "compdetection.h"

void tcp_client(struct config_details config, char *buf1, char *buf2, int post_probing) {
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	char *port_num = config.port_tcp_pre;
	if (post_probing) {
		port_num = config.port_tcp_post;
	}
	int err_check = getaddrinfo(config.server_ip, port_num, &hints, &res);
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

	if (!post_probing) {
		int sent = send(fd, buf1, strlen(buf1), 0);
		if (sent < (int) strlen(buf1)) {
			error_detail("failed to send msg 1");
		}
	} else {
		// Accept two msgs: low_ent and high_ent differences
		int rec = recv(fd, buf1, ENT_MSG_SIZE, 0);
		if (rec == -1) {
			error(rec);
		} else if (rec == 0) {
			printf("Connection closed by foreign host\n");
		}
		rec = recv(fd, buf2, ENT_MSG_SIZE, 0);
		if (rec == -1) {
			error(rec);
		} else if (rec == 0) {
			printf("Connection closed by foreign host\n");
		}
	}
	close(fd); 
}

void tcp_server(char *port_num, char *buf1, char *buf2, int post_probing) {
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

	if (!post_probing) {
		int rec = recv(sfd, buf1, MAX_CONFIG_SIZE, 0);
		if (rec == -1) {
			error(rec);
		} else if (rec == 0) {
			printf("Connection closed by foreign host\n");
		}
		buf1[rec] = '\0';
	} else {
		int bytes_sent = send(sfd, buf1, ENT_MSG_SIZE, 0);
		if (bytes_sent < (int) strlen(buf1)) {
			error_detail("failed to send msg 1");
		}
		bytes_sent = send(sfd, buf2, ENT_MSG_SIZE, 0);
		if (bytes_sent < (int) strlen(buf2)) {
			error_detail("failed to send msg 2");
		}
	}
}
