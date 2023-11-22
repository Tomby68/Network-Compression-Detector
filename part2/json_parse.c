#include "standalone.h"

char *read_json_key(cJSON *json, char *name) {
	cJSON *key = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!cJSON_IsString(key) || key->valuestring == NULL) {
		jsonError(cJSON_GetErrorPtr(), json);
	}
	return key->valuestring;
}

void read_config_from_file(char *fileName, struct config_details *config, char *buf) {
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
	read_config(config, buf);
}

void read_config(struct config_details *config, char *buf) {

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
