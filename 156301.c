mm_request_send(int socket, enum monitor_reqtype type, Buffer *m)
{
	u_int mlen = buffer_len(m);
	u_char buf[5];

	debug3("%s entering: type %d", __func__, type);

	PUT_32BIT(buf, mlen + 1);
	buf[4] = (u_char) type;		/* 1st byte of payload is mesg-type */
	if (atomicio(write, socket, buf, sizeof(buf)) != sizeof(buf))
		fatal("%s: write", __func__);
	if (atomicio(write, socket, buffer_ptr(m), mlen) != mlen)
		fatal("%s: write", __func__);
}