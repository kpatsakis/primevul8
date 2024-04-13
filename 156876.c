inet_sockaddrtotrio(struct sockaddr_storage *addr, uint16_t proto)
{
	char addr_str[INET6_ADDRSTRLEN];
	static char ret[sizeof(addr_str) + 13];	/* '[' + addr_str + ']' + ':' + 'sctp' + ':' + 'nnnnn' */
	char *proto_str = proto == IPPROTO_TCP ? "tcp" : proto == IPPROTO_UDP ? "udp" : proto == IPPROTO_SCTP ? "sctp" : proto == 0 ? "none" : "?";

	inet_sockaddrtos2(addr, addr_str);
	snprintf(ret, sizeof(ret), "[%s]:%s:%d" ,addr_str, proto_str,
		 ntohs(inet_sockaddrport(addr)));
	return ret;
}