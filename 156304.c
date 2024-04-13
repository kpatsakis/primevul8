mm_request_receive_expect(int socket, enum monitor_reqtype type, Buffer *m)
{
	u_char rtype;

	debug3("%s entering: type %d", __func__, type);

	mm_request_receive(socket, m);
	rtype = buffer_get_char(m);
	if (rtype != type)
		fatal("%s: read: rtype %d != type %d", __func__,
		    rtype, type);
}