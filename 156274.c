mm_request_receive(int socket, Buffer *m)
{
	u_char buf[4];
	u_int msg_len;
	ssize_t res;

	debug3("%s entering", __func__);

	res = atomicio(read, socket, buf, sizeof(buf));
	if (res != sizeof(buf)) {
		if (res == 0)
			fatal_cleanup();
		fatal("%s: read: %ld", __func__, (long)res);
	}
	msg_len = GET_32BIT(buf);
	if (msg_len > 256 * 1024)
		fatal("%s: read: bad msg_len %d", __func__, msg_len);
	buffer_clear(m);
	buffer_append_space(m, msg_len);
	res = atomicio(read, socket, buffer_ptr(m), msg_len);
	if (res != msg_len)
		fatal("%s: read: %ld != msg_len", __func__, (long)res);
}