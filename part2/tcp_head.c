#include "standalone.h"

int tcp_syn(char *port_num, char *server_ip) {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd == -1) {
		error(errno);
	}

	char packet[4096];

	struct ipheader *iphdr = (struct ipheader *) packet;
	struct tcpheader *tcphdr = (struct tcpheader *) (packet + sizeof(struct ipheader));
	struct sockaddr_in sin;
	struct pseudo_header psh;
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(port_num));
	sin.sin_addr.s_addr = inet_addr(server_ip);

	memset(packet, 0, 4096);

	// Fill in IPv4 header
	iphdr->iph_l = 5;
	iphdr->iph_v = 0b100;
	iphdr->iph_tos = 0;	
	iphdr->iph_tl = sizeof(struct ipheader) + sizeof(struct tcpheader);
	iphdr->iph_id = htonl(0);
	iphdr->iph_offset = 0;
	iphdr->iph_ttl = 255;
	iphdr->iph_proto = IPPROTO_TCP;
	iphdr->iph_sum = 0;
	iphdr->iph_src = inet_addr("127.0.0.1");
	iphdr->iph_dst = sin.sin_addr.s_addr;

	// Fill in TCP header
	tcphdr->th_sport = htons(1234);
	tcphdr->th_dport = htons(atoi(port_num));
	tcphdr->th_seq = random();
	tcphdr->th_ack = 0;
	tcphdr->th_x2 = 0;
	tcphdr->th_off = 5;
	tcphdr->th_flags = TH_SYN;
	tcphdr->th_win = htons(65535);
	tcphdr->th_sum = 0;
	tcphdr->th_urp = 0;

	// Fill in pseudoheader
	psh.source_address = inet_addr("127.0.0.1");
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcpheader));
	int psize = sizeof(struct pseudo_header) + sizeof(struct tcpheader);

	char *pseudogram = malloc(psize);
	memcpy(pseudogram, (char*)&psh, sizeof(struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header), tcphdr, sizeof(struct tcpheader));
	tcphdr->th_sum = checksum((unsigned short *) pseudogram, psize);
	iphdr->iph_sum = checksum((unsigned short *) packet, iphdr->iph_l >> 1);
	
	int one = 1;
	const int *val = &one;
	int err_check = setsockopt(fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one));
	if (err_check < 0) {
		printf("Warning: Cannot set HDRINCL!\n");
	}

	err_check = sendto(fd, packet, iphdr->iph_tl, 0, (struct sockaddr *) &sin, sizeof(sin));
	if (err_check < 0) {
		printf("error: errno = %i\n", errno);
	}
	return fd;
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
