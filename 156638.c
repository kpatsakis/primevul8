static void add_request_options(GDHCPClient *dhcp_client,
				struct dhcp_packet *packet)
{
	int len = 0;
	GList *list;
	uint8_t code;
	int end = dhcp_end_option(packet->options);

	for (list = dhcp_client->request_list; list; list = list->next) {
		code = (uint8_t) GPOINTER_TO_INT(list->data);

		packet->options[end + OPT_DATA + len] = code;
		len++;
	}

	if (len) {
		packet->options[end + OPT_CODE] = DHCP_PARAM_REQ;
		packet->options[end + OPT_LEN] = len;
		packet->options[end + OPT_DATA + len] = DHCP_END;
	}
}