int select_compression_request_header(struct session *s, struct buffer *req)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &txn->req;
	struct hdr_ctx ctx;
	struct comp_algo *comp_algo = NULL;
	struct comp_algo *comp_algo_back = NULL;

	/* Disable compression for older user agents announcing themselves as "Mozilla/4"
	 * unless they are known good (MSIE 6 with XP SP2, or MSIE 7 and later).
	 * See http://zoompf.com/2012/02/lose-the-wait-http-compression for more details.
	 */
	ctx.idx = 0;
	if (http_find_header2("User-Agent", 10, req->p, &txn->hdr_idx, &ctx) &&
	    ctx.vlen >= 9 &&
	    memcmp(ctx.line + ctx.val, "Mozilla/4", 9) == 0 &&
	    (ctx.vlen < 31 ||
	     memcmp(ctx.line + ctx.val + 25, "MSIE ", 5) != 0 ||
	     ctx.line[ctx.val + 30] < '6' ||
	     (ctx.line[ctx.val + 30] == '6' &&
	      (ctx.vlen < 54 || memcmp(ctx.line + 51, "SV1", 3) != 0)))) {
	    s->comp_algo = NULL;
	    return 0;
	}

	/* search for the algo in the backend in priority or the frontend */
	if ((s->be->comp && (comp_algo_back = s->be->comp->algos)) || (s->fe->comp && (comp_algo_back = s->fe->comp->algos))) {
		ctx.idx = 0;
		while (http_find_header2("Accept-Encoding", 15, req->p, &txn->hdr_idx, &ctx)) {
			for (comp_algo = comp_algo_back; comp_algo; comp_algo = comp_algo->next) {
				if (word_match(ctx.line + ctx.val, ctx.vlen, comp_algo->name, comp_algo->name_len)) {
					s->comp_algo = comp_algo;

					/* remove all occurrences of the header when "compression offload" is set */

					if ((s->be->comp && s->be->comp->offload) ||
					    (s->fe->comp && s->fe->comp->offload)) {
						http_remove_header2(msg, &txn->hdr_idx, &ctx);
						ctx.idx = 0;
						while (http_find_header2("Accept-Encoding", 15, req->p, &txn->hdr_idx, &ctx)) {
							http_remove_header2(msg, &txn->hdr_idx, &ctx);
						}
					}
					return 1;
				}
			}
		}
	}

	/* identity is implicit does not require headers */
	if ((s->be->comp && (comp_algo_back = s->be->comp->algos)) || (s->fe->comp && (comp_algo_back = s->fe->comp->algos))) {
		for (comp_algo = comp_algo_back; comp_algo; comp_algo = comp_algo->next) {
			if (comp_algo->add_data == identity_add_data) {
				s->comp_algo = comp_algo;
				return 1;
			}
		}
	}

	s->comp_algo = NULL;
	return 0;
}