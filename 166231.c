int http_replace_hdrs(struct stream* s, struct htx *htx, struct ist name,
		     const char *str, struct my_regex *re, int full)
{
	struct http_hdr_ctx ctx;
	struct buffer *output = get_trash_chunk();

	ctx.blk = NULL;
	while (http_find_header(htx, name, &ctx, full)) {
		if (!regex_exec_match2(re, ctx.value.ptr, ctx.value.len, MAX_MATCH, pmatch, 0))
			continue;

		output->data = exp_replace(output->area, output->size, ctx.value.ptr, str, pmatch);
		if (output->data == -1)
			return -1;
		if (!http_replace_header_value(htx, &ctx, ist2(output->area, output->data)))
			return -1;
	}
	return 0;
}