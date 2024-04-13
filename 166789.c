proto_get_frame_protocols(const wmem_list_t *layers, gboolean *is_ip,
			  gboolean *is_tcp, gboolean *is_udp,
			  gboolean *is_sctp, gboolean *is_tls,
			  gboolean *is_rtp,
			  gboolean *is_lte_rlc)
{
	wmem_list_frame_t *protos = wmem_list_head(layers);
	int	    proto_id;
	const char *proto_name;

	/* Walk the list of a available protocols in the packet and
	   attempt to find "major" ones. */
	/* It might make more sense to assemble and return a bitfield. */
	while (protos != NULL)
	{
		proto_id = GPOINTER_TO_INT(wmem_list_frame_data(protos));
		proto_name = proto_get_protocol_filter_name(proto_id);

		if (is_ip && ((!strcmp(proto_name, "ip")) ||
			      (!strcmp(proto_name, "ipv6")))) {
			*is_ip = TRUE;
		} else if (is_tcp && !strcmp(proto_name, "tcp")) {
			*is_tcp = TRUE;
		} else if (is_udp && !strcmp(proto_name, "udp")) {
			*is_udp = TRUE;
		} else if (is_sctp && !strcmp(proto_name, "sctp")) {
			*is_sctp = TRUE;
		} else if (is_tls && !strcmp(proto_name, "tls")) {
			*is_tls = TRUE;
		} else if (is_rtp && !strcmp(proto_name, "rtp")) {
			*is_rtp = TRUE;
		} else if (is_lte_rlc && !strcmp(proto_name, "rlc-lte")) {
			*is_lte_rlc = TRUE;
		}

		protos = wmem_list_frame_next(protos);
	}
}