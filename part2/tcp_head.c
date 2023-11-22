#include "standalone.h"

void tcp_syn(char *port_num, char *server_ip) {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd == -1) {
		error(errno);
	}

	char packet[4096];

	struct ipheader *iphdr = (struct ipheader *) packet;
	struct tcpheader *tcphdr = (struct tcpheader *) packet + sizeof(struct ipheader);
	struct sockaddr_in sin;
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(port_num));
	sin.sin_addr.s_addr = inet_addr(server_ip);

	memset(packet, 0, 4096);

	iphdr->iph_v = 4;
	iphdr->iph_l = 5;
	iphdr->iph_tos = 0;	
	iphdr->iph_tl = sizeof(struct ipheader) + sizeof(struct tcpheader);
	iphdr->iph_id = 0;
	iphdr->iph_offset = 0;
	iphdr->iph_ttl = 255;
	iphdr->iph_proto = IPPROTO_TCP;
	iphdr->iph_sum = 0;
	iphdr->iph_src = inet_addr("1.2.3.4");
	iphdr->iph_dst = sin.sin_addr.s_addr;

	tcphdr->th_sport = htons(1234);
	tcphdr->th_dport = htons(atoi(port_num));
	tcphdr->th_seq = random();
	tcphdr->th_ack = 0;
	tcphdr->th_x2 = 0;
	tcphdr->th_off = 0;
	tcphdr->th_flags = TH_SYN;
	tcphdr->th_win = htons(65535);
	tcphdr->th_sum = 0;
	tcphdr->th_urp = 0;

	iphdr->iph_sum = checksum((unsigned short *) packet, iphdr->iph_l >> 1);
	
	int one = 1;
	const int *val = &one;
	setsockopt(fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one));

	sendto(fd, packet, iphdr->iph_l, 0, (struct sockaddr *) &sin, sizeof(sin));
	
}

/* this function generates header checksums */
unsigned short checksum (unsigned short *buf, int nwords) {
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}
