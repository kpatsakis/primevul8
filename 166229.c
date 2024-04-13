int http_res_set_status(unsigned int status, struct ist reason, struct stream *s)
{
	struct htx *htx = htxbuf(&s->res.buf);
	char *res;

	chunk_reset(&trash);
	res = ultoa_o(status, trash.area, trash.size);
	trash.data = res - trash.area;

	/* Do we have a custom reason format string? */
	if (!isttest(reason)) {
		const char *str = http_get_reason(status);
		reason = ist(str);
	}

	if (!http_replace_res_status(htx, ist2(trash.area, trash.data), reason))
		return -1;
	return 0;
}