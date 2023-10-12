#include "compdetection.h"

void malloc_error() {
	printf("Failed to malloc\n");
	exit(-1);
}

void read_config(char *fileName, struct config_details *config) {
	FILE *f = fopen(fileName, "r");
	if (!f) {
		printf("failed to open %s\n", fileName);
		exit(-1);
	}

	char *next_line = malloc(sizeof(char *));
	if (next_line == NULL) {
		malloc_error();
	}
	memset(next_line, '0', 32);
	fgets(next_line, 127, f);

	char *value = "";
	char *key = "";
	
	while (next_line != NULL) {
		value = next_line;
		key = strsep(&value, ":");
		if (key == NULL || value == NULL) {
			break;
		}
		
		if (!strncmp(key, "server_ip", strlen(key))) {
			strncpy(config->server_ip, value, 32);
		} else if (!strcmp(key, "source_port_udp")) {
			config->source_port_udp = atoi(value);
		} else if (!strcmp(key, "dest_port_udp")) {
			config->dest_port_udp = atoi(value);
		} else if (!strcmp(key, "dest_port_tcp_head")) {
			config->dest_port_tcp_head = atoi(value);
		} else if (!strcmp(key, "dest_port_tcp_tail")) {
			config->dest_port_tcp_tail = atoi(value);
		} else if (!strcmp(key, "port_tcp_pre")) {
			strncpy(config->port_tcp_pre, value, 32);
		} else if (!strcmp(key, "port_tcp_post")) {
			config->port_tcp_post = atoi(value);
		} else if (!strcmp(key, "udp_payload")) {
			config->udp_payload = atoi(value);
		} else if (!strcmp(key, "inter_measurement_time")) {
			config->inter_measurement_time = atoi(value);
		} else if (!strcmp(key, "udp_num")) {
			config->udp_num = atoi(value);
		} else if (!strcmp(key, "udp_ttl")) {
			config->udp_ttl = atoi(value);
		} 
		next_line = fgets(next_line, 127, f);
	}  

	int err = fclose(f);
	if (err) {
		printf("Failed to close %s\n", fileName);
		exit(-1);
	}
	free(next_line);
}

int initialize_tcp(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(NULL, config.port_tcp_pre, &hints, &res);
	if (!err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_family, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}



	freeaddrinfo(res);
}

int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name");
		exit(-1);
	}
	struct config_details config;
	
	read_config(argv[1], &config);

	int fd;

	fd = initialize_tcp(config);
	
	return 0;	// Success
	
}
