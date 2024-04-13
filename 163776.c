int http_send_name_header(struct http_txn *txn, struct proxy* be, const char* srv_name) {

	struct hdr_ctx ctx;

	char *hdr_name = be->server_id_hdr_name;
	int hdr_name_len = be->server_id_hdr_len;
	struct channel *chn = txn->req.chn;
	char *hdr_val;
	unsigned int old_o, old_i;

	ctx.idx = 0;

	old_o = chn->buf->o;
	if (old_o) {
		/* The request was already skipped, let's restore it */
		b_rew(chn->buf, old_o);
	}

	old_i = chn->buf->i;
	while (http_find_header2(hdr_name, hdr_name_len, txn->req.chn->buf->p, &txn->hdr_idx, &ctx)) {
		/* remove any existing values from the header */
	        http_remove_header2(&txn->req, &txn->hdr_idx, &ctx);
	}

	/* Add the new header requested with the server value */
	hdr_val = trash.str;
	memcpy(hdr_val, hdr_name, hdr_name_len);
	hdr_val += hdr_name_len;
	*hdr_val++ = ':';
	*hdr_val++ = ' ';
	hdr_val += strlcpy2(hdr_val, srv_name, trash.str + trash.size - hdr_val);
	http_header_add_tail2(&txn->req, &txn->hdr_idx, trash.str, hdr_val - trash.str);

	if (old_o) {
		/* If this was a forwarded request, we must readjust the amount of
		 * data to be forwarded in order to take into account the size
		 * variations. Note that if the request was already scheduled for
		 * forwarding, it had its req->sol pointing to the body, which
		 * must then be updated too.
		 */
		txn->req.sol += chn->buf->i - old_i;
		b_adv(chn->buf, old_o + chn->buf->i - old_i);
	}

	return 0;
}