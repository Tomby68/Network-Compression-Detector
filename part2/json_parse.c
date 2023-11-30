#include "standalone.h"

// Bring in args from main so it is not redeclared in standalone.h
extern struct arg_struct args[512];

/* Read individual json keys, and return the value if it exists
 * args:
 * cJSON *json: A cJSON object, which stores the .json file as key-value pairs
 * char *name: The key name of a .json object
 */

char *read_json_key(cJSON *json, char *name) {
	cJSON *key = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!cJSON_IsString(key) || key->valuestring == NULL) {
		jsonError(cJSON_GetErrorPtr(), json);
	}
	return key->valuestring;
}

/* Parse the json file given by char *fileName, and populate config with the parameters
 * args:
 * char *file_name: The .json file to parse
 * struct config_details *config: The struct to populate from the given file
 * char *buf: A placeholder to hold the file contents while parsing
 */
void read_config_from_file(char *file_name, struct config_details *config, char *buf) {
	FILE *f = fopen(file_name, "r");

	if (f == NULL) {
		printf("failed to open %s\n", file_name);
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

/* Populate the config struct with values from the .json file
 * args:
 * struct config_details *config: The struct to populate
 * char *buf: The .json file contents, un-formatted
 */
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
