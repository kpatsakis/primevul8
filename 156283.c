int mm_answer_auth2_read_banner(int socket, Buffer *m)
{
	char *banner;

	buffer_clear(m);
	banner = auth2_read_banner();
	buffer_put_cstring(m, banner != NULL ? banner : "");
	mm_request_send(socket, MONITOR_ANS_AUTH2_READ_BANNER, m);

	if (banner != NULL)
		xfree(banner);

	return (0);
}