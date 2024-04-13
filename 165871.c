static inline int ping_supported(int family, int type, int code)
{
	return (family == AF_INET && type == ICMP_ECHO && code == 0) ||
	       (family == AF_INET6 && type == ICMPV6_ECHO_REQUEST && code == 0);
}