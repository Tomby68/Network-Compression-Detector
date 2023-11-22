PROGS = part2_standalone
OBJS = cJSON.o standalone.o error.o tcp_head.o json_parse.o

%.o: %.c
	gcc -c -g -o $@ $<

$(PROGS): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(PROG) $(OBJS)
