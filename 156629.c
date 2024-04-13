static void add_binary_option(gpointer key, gpointer value, gpointer user_data)
{
	uint8_t *option = value;
	struct dhcp_packet *packet = user_data;

	dhcp_add_binary_option(packet, option);
}