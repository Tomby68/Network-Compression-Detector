PROGS = part2_standalone
OBJS = cJSON.o standalone.o error.o tcp_head.o json_parse.o udp_send.o rst_listener.o

%.o: %.c
	gcc -c -g -o $@ $< -pthread

$(PROGS): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(PROGS) $(OBJS)
