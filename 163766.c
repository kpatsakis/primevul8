unsigned int http_get_hdr(const struct http_msg *msg, const char *hname, int hlen,
			  struct hdr_idx *idx, int occ,
			  struct hdr_ctx *ctx, char **vptr, int *vlen)
{
	struct hdr_ctx local_ctx;
	char *ptr_hist[MAX_HDR_HISTORY];
	int len_hist[MAX_HDR_HISTORY];
	unsigned int hist_ptr;
	int found;

	if (!ctx) {
		local_ctx.idx = 0;
		ctx = &local_ctx;
	}

	if (occ >= 0) {
		/* search from the beginning */
		while (http_find_header2(hname, hlen, msg->chn->buf->p, idx, ctx)) {
			occ--;
			if (occ <= 0) {
				*vptr = ctx->line + ctx->val;
				*vlen = ctx->vlen;
				return 1;
			}
		}
		return 0;
	}

	/* negative occurrence, we scan all the list then walk back */
	if (-occ > MAX_HDR_HISTORY)
		return 0;

	found = hist_ptr = 0;
	while (http_find_header2(hname, hlen, msg->chn->buf->p, idx, ctx)) {
		ptr_hist[hist_ptr] = ctx->line + ctx->val;
		len_hist[hist_ptr] = ctx->vlen;
		if (++hist_ptr >= MAX_HDR_HISTORY)
			hist_ptr = 0;
		found++;
	}
	if (-occ > found)
		return 0;
	/* OK now we have the last occurrence in [hist_ptr-1], and we need to
	 * find occurrence -occ, so we have to check [hist_ptr+occ].
	 */
	hist_ptr += occ;
	if (hist_ptr >= MAX_HDR_HISTORY)
		hist_ptr -= MAX_HDR_HISTORY;
	*vptr = ptr_hist[hist_ptr];
	*vlen = len_hist[hist_ptr];
	return 1;
}