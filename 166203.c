int http_req_replace_stline(int action, const char *replace, int len,
			    struct proxy *px, struct stream *s)
{
	struct htx *htx = htxbuf(&s->req.buf);

	switch (action) {
		case 0: // method
			if (!http_replace_req_meth(htx, ist2(replace, len)))
				return -1;
			break;

		case 1: // path
			if (!http_replace_req_path(htx, ist2(replace, len), 0))
				return -1;
			break;

		case 2: // query
			if (!http_replace_req_query(htx, ist2(replace, len)))
				return -1;
			break;

		case 3: // uri
			if (!http_replace_req_uri(htx, ist2(replace, len)))
				return -1;
			break;

		case 4: // path + query
			if (!http_replace_req_path(htx, ist2(replace, len), 1))
				return -1;
			break;

		default:
			return -1;
	}
	return 0;
}