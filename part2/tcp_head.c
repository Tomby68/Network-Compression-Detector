#include "standalone.h"

#define SRC_IP "10.7.27.158"
int tcp_syn(char *port_num, char *server_ip) {
	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (fd == -1) {
		printf("Failed to open socket\n");
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
	iphdr->iph_src = inet_addr(SRC_IP);
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
	psh.source_address = inet_addr(SRC_IP);
	psh.dest_address = sin.sin_addr.s_addr;
	psh.placeholder = 0;
	psh.protocol = IPPROTO_TCP;
	psh.tcp_length = htons(sizeof(struct tcpheader));
	int psize = sizeof(struct pseudo_header) + sizeof(struct tcpheader);

	char *pseudogram = malloc(psize);
	memcpy(pseudogram, (char*)&psh, sizeof(struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header), tcphdr, sizeof(struct tcpheader));
	tcphdr->th_sum = checksum((const char*) pseudogram, psize);
	iphdr->iph_sum = checksum((const char*) packet, iphdr->iph_tl);
	
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
unsigned short checksum(const char *buf, unsigned size) {
	unsigned sum = 0, i;

	/* Accumulate checksum */
	for (i = 0; i < size - 1; i += 2)
	{
		unsigned short word16 = *(unsigned short *) &buf[i];
		sum += word16;
	}

	/* Handle odd-sized case */
	if (size & 1)
	{
		unsigned short word16 = (unsigned char) buf[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

	/* Invert to get the negative in ones-complement arithmetic */
	return ~sum;
}
