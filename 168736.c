static BOOL rdg_send_http_request(rdpRdg* rdg, rdpTls* tls, const char* method,
                                  const char* transferEncoding)
{
	size_t sz;
	wStream* s = NULL;
	int status = -1;
	s = rdg_build_http_request(rdg, method, transferEncoding);

	if (!s)
		return FALSE;

	sz = Stream_Length(s);

	if (sz <= INT_MAX)
		status = tls_write_all(tls, Stream_Buffer(s), (int)sz);

	Stream_Free(s, TRUE);
	return (status >= 0);
}