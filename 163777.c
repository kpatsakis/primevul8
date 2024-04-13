int http_find_header(const char *name,
		     char *sol, struct hdr_idx *idx,
		     struct hdr_ctx *ctx)
{
	return http_find_header2(name, strlen(name), sol, idx, ctx);
}