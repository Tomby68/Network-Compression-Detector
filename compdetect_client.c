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

int main(int argc, char *argv[]) {
	// Handle input
	if (argc != 2) {
		printf("Usage: ./compdetect_client file_name\n");
		exit(-1);
	}
	struct config_details config;

	char file_contents[1024];
	
	read_config(argv[1], &config, file_contents);

	init_tcp_client(config, file_contents); 

	printf("file_contents = %s\n", file_contents);
	
	return 0;	// Success
	
}
