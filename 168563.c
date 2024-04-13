ncp_update_known_namespace(struct ncp_server *server, __u8 volume, int *ret_ns)
{
	int ns = ncp_get_known_namespace(server, volume);

	if (ret_ns)
		*ret_ns = ns;

	ncp_dbg(1, "namespace[%d] = %d\n", volume, server->name_space[volume]);

	if (server->name_space[volume] == ns)
		return 0;
	server->name_space[volume] = ns;
	return 1;
}