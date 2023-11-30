#include "compdetection.h"

/* If the post_probing flag is not set, initialize a TCP connection with the server and send the
 * contents of the config file. If the post_probing flag is set, accept a TCP connection from the server
 * and receive the timestamps of the UDP packet trains
 * args:
 * struct config_details config: The config struct containing details about the server
 * char *buf1: Either the contents of the config file or a buffer for a UDP timestamp, based on the post_probing flag
 * char *buf2: Either an empty buffer or a buffer for a UDP timestamp, based on the post_probing flag
 * int post_probing: A flag. If not set, the client initializes a TCP connection. If set, the client accepts a TCP connection
 */
void tcp_client(struct config_details config, char *buf1, char *buf2, int post_probing) {
	// Initialize the structs needed for getaddrinfo()
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Initialize the port_num based on the post_probing flag
	char *port_num = config.port_tcp_pre;
	if (post_probing) {
		port_num = config.port_tcp_post;
	}

	// Populate the structs with getaddrinfo()
	int err_check = getaddrinfo(config.server_ip, port_num, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	// Open and connect the socket
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}
	err_check = connect(fd, res->ai_addr, res->ai_addrlen);
	if (err_check == -1) {
		error(errno);
	}
	freeaddrinfo(res);

	// If the post_probing flag is not sent, send the contents of the config file over TCP
	if (!post_probing) {
		int sent = send(fd, buf1, strlen(buf1), 0);
		if (sent < (int) strlen(buf1)) {
			error_detail("failed to send msg 1");
		}
	} else {
		// If the post_probing flag is set,
		// accept two messages over TCP: low_ent and high_ent differences
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

/* If the post_probing flag is not set, the server accepts a TCP connection from the client and receives
 * the contents of the config file. If the post_probing flag is set, the server initializes a TCP connection
 * with the client and sends the timestamps of the UDP packet trains
 *
 * args:
 * char *port_num: The port to send or receive over
 * char *buf1: Either a buffer for the config file contents, or a buffer containing a UDP timestamp, depending on the post_probing flag
 * char *buf2: Either an empty buffer, or a buffer containing a UDP timestamp, depending on the post_probing flag
 * int post_probing: A flag. If not set, the server accepts a TCP connection. If set, the server initializes a TCP connection
 */
void tcp_server(char *port_num, char *buf1, char *buf2, int post_probing) {
	// Initialize the structs needed for getaddrinfo()
	struct addrinfo hints;
	struct addrinfo *res;
	struct sockaddr_storage client_addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Populate the structs with getaddrinfo()
	int err_check = getaddrinfo(NULL, port_num, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	// Open, bind, listen, and accept over the socket
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

	// If the post_probing flag is not set, accept data over a TCP connection
	if (!post_probing) {
		int rec = recv(sfd, buf1, MAX_CONFIG_SIZE, 0);
		if (rec == -1) {
			error(rec);
		} else if (rec == 0) {
			printf("Connection closed by foreign host\n");
		}
		buf1[rec] = '\0';
	} else {
		// If the post_probing flag is set, send data over the TCP connection
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
