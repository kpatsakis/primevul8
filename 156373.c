mm_send_debug(Buffer *m)
{
	char *msg;

	while (buffer_len(m)) {
		msg = buffer_get_string(m, NULL);
		debug3("%s: Sending debug: %s", __func__, msg);
		packet_send_debug("%s", msg);
		xfree(msg);
	}
}