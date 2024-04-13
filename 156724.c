static int dhcp_l2_socket(int ifindex)
{
	int fd;
	struct sockaddr_ll sock;

	/*
	 * Comment:
	 *
	 *	I've selected not to see LL header, so BPF doesn't see it, too.
	 *	The filter may also pass non-IP and non-ARP packets, but we do
	 *	a more complete check when receiving the message in userspace.
	 *
	 * and filter shamelessly stolen from:
	 *
	 *	http://www.flamewarmaster.de/software/dhcpclient/
	 *
	 * There are a few other interesting ideas on that page (look under
	 * "Motivation").  Use of netlink events is most interesting.  Think
	 * of various network servers listening for events and reconfiguring.
	 * That would obsolete sending HUP signals and/or make use of restarts.
	 *
	 * Copyright: 2006, 2007 Stefan Rompf <sux@loplof.de>.
	 * License: GPL v2.
	 *
	 * TODO: make conditional?
	 */
	static const struct sock_filter filter_instr[] = {
		/* check for udp */
		BPF_STMT(BPF_LD|BPF_B|BPF_ABS, 9),
		/* L5, L1, is UDP? */
		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, IPPROTO_UDP, 2, 0),
		/* ugly check for arp on ethernet-like and IPv4 */
		BPF_STMT(BPF_LD|BPF_W|BPF_ABS, 2), /* L1: */
		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0x08000604, 3, 4),/* L3, L4 */
		/* skip IP header */
		BPF_STMT(BPF_LDX|BPF_B|BPF_MSH, 0), /* L5: */
		/* check udp source and destination ports */
		BPF_STMT(BPF_LD|BPF_W|BPF_IND, 0),
		/* L3, L4 */
		BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, SERVER_AND_CLIENT_PORTS, 0, 1),
		/* returns */
		BPF_STMT(BPF_RET|BPF_K, 0x0fffffff), /* L3: pass */
		BPF_STMT(BPF_RET|BPF_K, 0), /* L4: reject */
	};

	static const struct sock_fprog filter_prog = {
		.len = sizeof(filter_instr) / sizeof(filter_instr[0]),
		/* casting const away: */
		.filter = (struct sock_filter *) filter_instr,
	};

	fd = socket(PF_PACKET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return -errno;

	if (SERVER_PORT == 67 && CLIENT_PORT == 68)
		/* Use only if standard ports are in use */
		setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter_prog,
							sizeof(filter_prog));

	memset(&sock, 0, sizeof(sock));
	sock.sll_family = AF_PACKET;
	sock.sll_protocol = htons(ETH_P_IP);
	sock.sll_ifindex = ifindex;

	if (bind(fd, (struct sockaddr *) &sock, sizeof(sock)) != 0) {
		int err = -errno;
		close(fd);
		return err;
	}

	return fd;
}