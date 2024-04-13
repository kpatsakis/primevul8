static wStream* rdg_build_http_request(rdpRdg* rdg, const char* method,
                                       const char* transferEncoding)
{
	wStream* s = NULL;
	HttpRequest* request = NULL;
	const char* uri;

	if (!rdg || !method)
		return NULL;

	uri = http_context_get_uri(rdg->http);
	request = http_request_new();

	if (!request)
		return NULL;

	if (!http_request_set_method(request, method) || !http_request_set_uri(request, uri))
		goto out;

	if (rdg->ntlm)
	{
		if (!rdg_set_ntlm_auth_header(rdg->ntlm, request))
			goto out;
	}

	if (transferEncoding)
	{
		http_request_set_transfer_encoding(request, transferEncoding);
	}

	s = http_request_write(rdg->http, request);
out:
	http_request_free(request);

	if (s)
		Stream_SealLength(s);

	return s;
}