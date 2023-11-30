PROGS = compdetect_client compdetect_server
CLIENT_OBJS = compdetect_client.o error.o cJSON.o tcp.o json_parse.o udp.o
SERVER_OBJS = compdetect_server.o error.o cJSON.o tcp.o json_parse.o udp.o

%.o: %.c
	gcc -c -g -o $@ $<

all: $(PROGS)

compdetect_client: $(CLIENT_OBJS)
	gcc -g -o $@ $^

compdetect_server: $(SERVER_OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(PROGS) $(CLIENT_OBJS) $(SERVER_OBJS)
