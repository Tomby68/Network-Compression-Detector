PROGS = compdetection_client compdetection_server
CLIENT_OBJS = compdetect_client.o error.o cJSON.o tcp.o json_parse.o udp.o
SERVER_OBJS = compdetect_server.o error.o cJSON.o tcp.o json_parse.o udp.o

%.o: %.c
	gcc -c -g -o $@ $<

all: $(PROGS)

compdetection_client: $(CLIENT_OBJS)
	gcc -g -o $@ $^

compdetection_server: $(SERVER_OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(PROGS) $(CLIENT_OBJS) $(SERVER_OBJS)
