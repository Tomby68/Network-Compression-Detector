#include "compdetection.h"

#define MAX_CONFIG_SIZE 1024

char *read_json_key(cJSON *json, char *name) {
	cJSON *key = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!cJSON_IsString(key) || key->valuestring == NULL) {
		jsonError(cJSON_GetErrorPtr(), json);
	}
	return key->valuestring;
}

void read_config(char *fileName, struct config_details *config, char *buf) {
	FILE *f = fopen(fileName, "r");

	if (f == NULL) {
		printf("failed to open %s\n", fileName);
		exit(-1);
	}
	
	int len = fread(buf, 1, MAX_CONFIG_SIZE, f);
	if (len == 0) {
		if (!feof(f)) {
			error(ferror(f));
		}
	}
	fclose(f);

	cJSON *json = cJSON_Parse(buf);
	if (json == NULL) {
		jsonError(cJSON_GetErrorPtr(), json);
	}

	config->server_ip = read_json_key(json, "server_ip");
	config->source_port_udp = read_json_key(json, "source_port_udp");
	config->dest_port_udp = read_json_key(json, "dest_port_udp");
	config->dest_port_tcp_head = read_json_key(json, "dest_port_tcp_head");
	config->dest_port_tcp_tail = read_json_key(json, "dest_port_tcp_tail");
	config->port_tcp_pre = read_json_key(json, "port_tcp_pre");
	config->port_tcp_post = read_json_key(json, "port_tcp_post");
	config->udp_payload = read_json_key(json, "udp_payload");
	config->inter_measurement_time = read_json_key(json, "inter_measurement_time");
	config->udp_num = read_json_key(json, "udp_num");
	config->udp_ttl = read_json_key(json, "udp_ttl");
}

void initialize_tcp(struct config_details config) {
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err_check = getaddrinfo(NULL, config.port_tcp_pre, &hints, &res);
	if (err_check) {
		error_gai(err_check);
	}

	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd == -1) {
		error(fd);
	}

	err_check = connect(fd, res->ai_addr, res->ai_protocol);
	if (err_check == -1) {
		error(errno);
	}

	int sent = send(fd, "Hi", 2, 0);
	if (sent < 2) {
		error_detail("failed to send");
	}

	close(fd);
	freeaddrinfo(res);
	//return fd;
}

int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	struct config_details config;

	char file_contents[1024];
	
	read_config(argv[1], &config, file_contents);

	initialize_tcp(config); 
	
	return 0;	// Success
	
}
