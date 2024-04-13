static bool server_remote_addr_filter(PgSocket *sk, void *arg) {
	PgAddr *addr = arg;

	return (pga_cmp_addr(&sk->remote_addr, addr) == 0);
}