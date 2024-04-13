proto_registrar_dump_protocols(void)
{
	protocol_t *protocol;
	int	    i;
	void	   *cookie = NULL;


	i = proto_get_first_protocol(&cookie);
	while (i != -1) {
		protocol = find_protocol_by_id(i);
		ws_debug_printf("%s\t%s\t%s\n", protocol->name, protocol->short_name,
			protocol->filter_name);
		i = proto_get_next_protocol(&cookie);
	}
}