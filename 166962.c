proto_get_first_protocol(void **cookie)
{
	protocol_t *protocol;

	if (protocols == NULL)
		return -1;
	*cookie = protocols;
	protocol = (protocol_t *)protocols->data;
	return protocol->proto_id;
}